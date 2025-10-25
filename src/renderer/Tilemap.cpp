#include <algorithm>

#include "glm/gtc/type_ptr.hpp"
#include "tmxlite/TileLayer.hpp"
#include "tmxlite/Map.hpp"

#include "Debug.h"
#include "renderer/Tilemap.h"
#include "renderer/gl/Shader.h"
#include "renderer/gl/Texture.h"
#include "renderer/gl/TilemapLayer.h"

namespace une
{
	Tilemap::Tilemap(const std::string& path, unsigned int filteringType)
	{
		tmx::Map map;
		map.load(path);

		bounds = map.getBounds();
		tileSize = Vector2(map.getTileSize().x, map.getTileSize().y);

		//Process all tiles in each tileset
		for (const tmx::Tileset& tileset: map.getTilesets())
		{
			//Load the tileset's texture
			tilesetTextures.push_back(new Texture(tileset.getImagePath(), filteringType, false));

			//Get the collider shapes for each tile id
			for (const tmx::Tileset::Tile& tile: tileset.getTiles())
			{
				if (!tile.objectGroup.getObjects().empty())
				{
					//Only one collider per tile is currently supported
					//Get the first object in a tile to be used as a collider
					for (const tmx::Vector2f& point: tile.objectGroup.getObjects()[0].getPoints())
					{
						//Transform the Vertex to work with our coordinate system
						Vector2 vertex(point.x, point.y);
						vertex.y = -vertex.y;
						vertex += Vector2((double)tileSize.x / 2, (double)tileSize.y / 2);
						tileColliders[tile.ID].push_back(vertex);
					}
				}
			}
		}

		//Process all the layers
		const auto& layers = map.getLayers();
		for (auto i = 0u; i < layers.size(); ++i)
		{
			//Currently we only support tile layers
			if (layers[i]->getType() == tmx::Layer::Type::Tile)
			{
				//Process the layer properties
				std::unordered_map<std::string, tmx::Property> layerProperties;
				for (const tmx::Property& property: layers[i]->getProperties())
				{
					//Convert all properties to lower case
					std::string propertyName;
					std::transform(property.getName().begin(), property.getName().end(), propertyName.begin(), tolower);
					layerProperties[propertyName] = property;
				}

				MapLayer* layer = new MapLayer(map, i, tilesetTextures, layerProperties);
				mapLayers.push_back(layer);
			}
		}

		//Initialize the OpenGL resources
		const float vertices[] = {
			bounds.left, bounds.top, 0.f, 0.f, 1.f,
			bounds.left + bounds.width, bounds.top, 0.f, 1.f, 1.f,
			bounds.left, bounds.top + bounds.height, 0.f, 0.f, 0.f,
			bounds.left + bounds.width, bounds.top + bounds.height, 0.f, 1.f, 0.f
		};

		//Make the single shared buffer
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		//Every layer will use the same vertex data
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		//Vertex positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		//Lookup texture coords
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	Tilemap::~Tilemap()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteVertexArrays(1, &VBO);
		for (Texture* tex : tilesetTextures)
			delete tex;
		for (MapLayer* layer : mapLayers)
			delete layer;
	}

	void Tilemap::SetLayerVisibility(unsigned int id, bool visible)
	{
		if (id >= mapLayers.size())
		{
			debug::LogWarning("Invalid layer id " + std::to_string(id));
			return;
		}

		mapLayers[id]->enabled = visible;
	}

	//Returns the vertices making up this tile's collider
	std::vector<Vector2> Tilemap::GetTileCollider(unsigned int id)
	{
		if (tileColliders.contains(id - 1))
		{
			return tileColliders[id - 1];
		} else
		{
			//Default collider of a tile
			return std::vector<Vector2>{
				Vector2(-((float) tileSize.x / 2), (float) tileSize.y / 2), //Top-Left
				Vector2((float) tileSize.x / 2, (float) tileSize.y / 2), //Top-Right
				Vector2((float) tileSize.x / 2, -((float) tileSize.y / 2)), //Bottom-Right
				Vector2(-((float) tileSize.x / 2), -((float) tileSize.y / 2)) //Bottom-Left
			};
		}
	}

	//Returns the id of every tile with a collider at tilemap coords x and y
	std::vector<unsigned int> Tilemap::GetCollisionTileAtLocation(unsigned int x, unsigned int y)
	{
		std::vector<unsigned int> hits;
		/*
		for (const auto& layer : layerColliders)
		{
			if (!layer.second.empty())
			{
				hits.push_back(layer.second[x][y]);
			}
		}
		*/
		return hits;
	}

	//Get the center position of a tile in world coordinates
	Vector2 Tilemap::GetTilePosition(unsigned int x, unsigned int y)
	{
		//Move from top left to center position
		Vector2 position;
		position.x += tileSize.x / 2;
		position.y -= tileSize.y / 2;

		//X increases in positive X
		position.x += x * tileSize.x;
		//Y increases in negative Y
		position.y -= y * tileSize.y;

		return position;
	}

	//Find any collision layer tiles in a box
	std::vector<Vector2> Tilemap::CheckCollisionBox(Vector2 topLeft, Vector2 bottomRight)
	{
		//Log all tiles
		std::vector<Vector2> collisions;
		/*
		if (collisionLayer.empty())
		{
			return collisions;
		}

		//Calculate the x and y index bounds
		int xMin = floor((topLeft.x + position.x) / tileSize.x);
		int xMax = ceil((bottomRight.x + position.x) / tileSize.x);
		int yMin = floor((-topLeft.y + position.y) / tileSize.y);
		int yMax = ceil((-bottomRight.y + position.y) / tileSize.y);

		//Check every position inside the box
		for (int x = xMin; x < xMax; x++)
		{
			for (int y = yMin; y < yMax; y++)
			{
				//Check bounds
				if (x < collisionLayer.size() && x >= 0 && y < collisionLayer[0].size() && y >= 0)
				{
					//If tile is in collision layer log it
					if (collisionLayer[x][y] != 0)
					{
						collisions.push_back(Vector2(x, y));
					}
				}
			}
		}
		*/
		return collisions;
	}

	//Returns the collision layers tile ID at x and y
	unsigned int Tilemap::checkCollision(float x, float y)
	{
		return 0;
		/*
		if (collisionLayer.empty())
			return 0;

		int xIndex = floor((x + position.x) / tileSize.x);
		int yIndex = floor((-y + position.y) / tileSize.y);

		//Check out of bounds
		if (xIndex >= collisionLayer.size() || yIndex >= collisionLayer[0].size() || xIndex < 0 || yIndex < 0)
			return 0;

		return collisionLayer[xIndex][yIndex];
		*/
	}

	namespace renderer
	{
		void TilemapRenderSystem::Init()
		{
			shader = new Shader(
				R"(
				#version 460 core
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
				#version 460 core
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

					if (tileID < 65535)
					{
						vec2 position = vec2(tileID % tilesetSize.x, floor(tileID / tilesetSize.x)) / tilesetSize;

						vec2 texelSize = vec2(1.0) / textureSize(lookupTexture, 0);
						vec2 offset = mod(TexCoord, texelSize);
						vec2 ratio = offset / texelSize;
						offset = ratio * (1.0 / tileSize);
						offset *= tileSize / tilesetSize;

						//Flip the tile based on flip flags
						if (flipFlags != 0)
						{
							vec2 tileSize = vec2(1.0) / tilesetSize;
							if ((flipFlags & FLIP_DIAGONAL) != 0)
							{/*
								float temp = offset.x;
								offset.x = offset.y;
								offset.y = temp;
								temp = tileSize.x / tileSize.y;
								offset.x *= temp;
								offset.y /= temp;
								offset.x = tileSize.x - offset.x;
								offset.y = tileSize.y - offset.y;*/
							}
							if ((flipFlags & FLIP_VERTICAL) != 0)
							{
								offset.y = tileSize.y - offset.y;
							}
							if ((flipFlags & FLIP_HORIZONTAL) != 0)
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
			for (ecs::Entity entity : entities)
			{
				TilemapRenderer& renderer = ecs::GetComponent<TilemapRenderer>(entity);
				Vector3 pos = TransformSystem::GetGlobalTransform(entity).position;

				if (!renderer.enabled)
					continue;

				for (const MapLayer* layer : renderer.tilemap->mapLayers)
				{
					if (!layer->enabled)
						continue;
					transparentWorldLayers.push_back({entity, pos + Vector3(0, 0, layer->zOffset), DrawRenderable, layer->index});
				}
			}
		}

		//Draws one layer of an entity's tilemap
		void TilemapRenderSystem::DrawLayer(ecs::Entity entity, Camera* cam, unsigned int id)
		{
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
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(cam->GetViewMatrix()));
			int projLoc = glGetUniformLocation(shader->ID, "projection");
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(cam->GetProjectionMatrix()));
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

		//Static version of DrawLayer for renderable
		void TilemapRenderSystem::DrawRenderable(const Renderable& r, Camera* cam)
		{
			ecs::GetSystem<TilemapRenderSystem>()->DrawLayer(r.entity, cam, r.index);
		}

		const std::vector<Renderable>& TilemapRenderSystem::GetTransparentWorldLayers()
		{
			return transparentWorldLayers;
		}
	}
}
