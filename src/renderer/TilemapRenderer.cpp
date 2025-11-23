#include <algorithm>

#include "glm/gtc/type_ptr.hpp"

#include "debug/Logging.h"
#include "renderer/TilemapRenderer.h"
#include "renderer/gl/Shader.h"

namespace une::renderer
{
	void TilemapRenderSystem::Init()
	{
		shader = new Shader(
			R"(
				#version 330 core
				layout(location = 0) in vec3 aPos;
				layout(location = 1) in vec2 aTexCoord;
				out vec2 TexCoord;
				uniform mat4 model;
				uniform mat4 view;
				uniform mat4 projection;
				void main()
				{
					gl_Position = projection * view * model * vec4(aPos, 1.0);
					TexCoord = aTexCoord;
				}
			)",
			R"(
				#version 330 core
				//Tiled flip flags
				#define FLIP_HORIZONTAL 8u
				#define FLIP_VERTICAL 4u
				#define FLIP_DIAGONAL 2u

				in vec2 TexCoord;

				uniform sampler2D tilesetTexture;
				uniform usampler2D lookupTexture;

				uniform uvec2 tileSize;
				uniform uvec2 tilesetSize;
				uniform float opacity = 1.0;

				out vec4 FragColor;

				void main()
				{
					uvec2 lookupValues = texture(lookupTexture, TexCoord).rg;
					uint tileID = lookupValues.r;
					uint flipFlags = lookupValues.g;

					if (tileID < 65535u)
					{
						vec2 position = vec2(tileID % tilesetSize.x, tileID / tilesetSize.x) / tilesetSize;

						vec2 texelSize = vec2(1.0) / textureSize(lookupTexture, 0);
						vec2 offset = mod(TexCoord, texelSize);
						vec2 ratio = offset / texelSize;
						offset = ratio * (1.0 / tileSize);
						offset *= tileSize / tilesetSize;

						//Flip the tile based on flip flags
						if (flipFlags != 0u)
						{
							vec2 tileSize = vec2(1.0) / tilesetSize;
							if ((flipFlags & FLIP_DIAGONAL) != 0u)
							{
								float temp = offset.x;
								offset.x = offset.y;
								offset.y = temp;
								temp = tileSize.x / tileSize.y;
								offset.x *= temp;
								offset.y /= temp;
								offset.x = tileSize.x - offset.x;
								offset.y = tileSize.y - offset.y;
							}
							if ((flipFlags & FLIP_VERTICAL) != 0u)
							{
								offset.y = tileSize.y - offset.y;
							}
							if ((flipFlags & FLIP_HORIZONTAL) != 0u)
							{
								offset.x = tileSize.x - offset.x;
							}
						}

						vec4 texColor = texture(tilesetTexture, position + offset);
						texColor.a *= opacity;
						FragColor = texColor;
					}
					else
					{
						FragColor = vec4(0);
					}
				}
				)", false);
	}

	//Sorts the tilemap layers into their draw layers (currently only transparent world)
	void TilemapRenderSystem::Prepass()
	{
		transparentWorldLayers.clear();

		//Sort all entities into their draw orders
		for (ecs::Entity entity: entities)
		{
			TilemapRenderer& renderer = ecs::GetComponent<TilemapRenderer>(entity);
			Vector3 pos = TransformSystem::GetGlobalTransform(entity).position;

			if (!renderer.enabled)
				continue;

			for (const MapLayer* layer: renderer.tilemap->mapLayers)
			{
				if (!layer->enabled)
					continue;
				transparentWorldLayers.push_back({entity, pos + Vector3(0, 0, layer->zOffset), DrawRenderable, layer->index});
			}
		}
	}

	//Static version of DrawLayer for renderable
	void TilemapRenderSystem::DrawRenderable(const Renderable& r, ecs::Entity cameraEntity)
	{
		ecs::GetSystem<TilemapRenderSystem>()->DrawLayer(r.entity, cameraEntity, r.index);
	}

	//Draws one layer of an entity's tilemap
	void TilemapRenderSystem::DrawLayer(ecs::Entity entity, ecs::Entity cameraEntity, unsigned int id)
	{
		Camera& cam = ecs::GetComponent<Camera>(cameraEntity);
		TilemapRenderer& tilemapRenderer = ecs::GetComponent<TilemapRenderer>(entity);

		if (!tilemapRenderer.tilemap)
		{
			debug::LogWarning("No tilemap given for TilemapRenderer of entity " + std::to_string(entity));
			return;
		}

		MapLayer* layer = tilemapRenderer.tilemap->mapLayers[id];

		shader->Use();

		//Create the model matrix and offset it by the layer zOffset
		glm::mat4 model = TransformSystem::GetGlobalTransformMatrix(entity);
		model = glm::translate(model, {0, 0, layer->zOffset});

		//Get and set uniforms
		int modelLoc = glGetUniformLocation(shader->ID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		int viewLoc = glGetUniformLocation(shader->ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(cam.view));
		int projLoc = glGetUniformLocation(shader->ID, "projection");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(cam.projection));
		int tileSizeLoc = glGetUniformLocation(shader->ID, "tileSize");
		glUniform2ui(tileSizeLoc, tilemapRenderer.tilemap->tileSize.x, tilemapRenderer.tilemap->tileSize.y);
		int tilesetSizeLoc = glGetUniformLocation(shader->ID, "tilesetSize");
		//Set the textures to their proper slots
		glUniform1i(glGetUniformLocation(shader->ID, "tilesetTexture"), 0);
		glUniform1i(glGetUniformLocation(shader->ID, "lookupTexture"), 1);

		//Draw all layer subsets with the same VAO
		glBindVertexArray(tilemapRenderer.tilemap->VAO);
		layer->DrawSubsets(tilesetSizeLoc);

		glBindVertexArray(0);
	}

	const std::vector<Renderable>& TilemapRenderSystem::GetTransparentWorldLayers()
	{
		return transparentWorldLayers;
	}
}
