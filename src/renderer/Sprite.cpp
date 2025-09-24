#include "renderer/Sprite.h"

#include <vector>
#include <set>
#include <functional>

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ECS.h"
#include "Transform.h"
#include "renderer/gl/Shader.h"
#include "renderer/gl/Texture.h"
#include "renderer/gl/Camera.h"
#include "renderer/Tilemap.h"

namespace engine
{
	SpriteRenderSystem::~SpriteRenderSystem()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteVertexArrays(1, &VBO);
		glDeleteVertexArrays(1, &EBO);
	}
	///Initialize the shaders and clear the screen
	void SpriteRenderSystem::Init()
	{
		//Set the screen clear color to black
		glClearColor(0, 0, 0, 1.0f);

		//Enable transparency
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		//Enable Depth buffering
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		//Create the default sprite shader
		defaultShader = new Shader(
			R"(
			#version 330 core
			layout (location = 0) in vec3 aPos;
			layout(location = 1) in vec2 aTexCoord;
			out vec2 TexCoord;
			uniform mat4 model;
			uniform mat4 view;
			uniform mat4 projection;
			void main()
			{
				gl_Position = projection * view * model * vec4(aPos, 1.0f);
				TexCoord = vec2(aTexCoord.x, aTexCoord.y);
			}
			)",
			R"(
			#version 330 core
			out vec4 FragColor;
			in vec2 TexCoord;
			uniform sampler2D texture1;
			void main()
			{
				vec4 texColor = texture(texture1, TexCoord);
				if(texColor.a < 0.02)
					discard;
				FragColor = texColor;
			}
			)", false);

		//Rectangle vertices start at top left and go clockwise to bottom left
		float vertices[] = {
			//Positions		  Texture Coords
			 1.f,  1.f, 0.0f, 1.0f, 1.0f, // top right
			 1.f, -1.f, 0.0f, 1.0f, 0.0f, // bottom right
			-1.f, -1.f, 0.0f, 0.0f, 0.0f, // bottom left
			-1.f,  1.f, 0.0f, 0.0f, 1.0f, // top left
		};
		//Indices to draw a rectangle from two triangles
		unsigned int indices[] = {
			0, 1, 2, //1st trangle
			0, 2, 3, //2nd triangle
		};

		//Make the Vertex Array Object, Vertex Buffer Object, and Element Buffer Object
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		//Bind the Vertex Array Object
		glBindVertexArray(VAO);

		//Bind the Vertex Bufer Object and set vertices
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		//Bind and set indices to EBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		//Configure Vertex attribute at location 0 aka position
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		//Configure Vertex attribute at location 1 aka texture coords
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		//Unbind all buffers and arrays
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	///Renders everything. Call this every frame
	void SpriteRenderSystem::Update(Camera* cam)
	{
		//Sort the entities and tilemap by Z
		std::set<float> layersToDraw;
		std::map<float, std::vector<ecs::Entity>> sortedEntities;
		if (tilemap)
			layersToDraw.insert(tilemap->zLayers.begin(), tilemap->zLayers.end());

		//UI elements are sorted seperately
		std::set<float> uiLayersToDraw;
		std::map<float, std::vector<ecs::Entity>> sortedUIElements;

		//Sort the entities into sprite and UI layers
		for (ecs::Entity entity : entities)
		{
			Transform transform = TransformSystem::GetGlobalTransform(entity);
			SpriteRenderer& renderer = ecs::GetComponent<SpriteRenderer>(entity);

			//Seperate sprites and UI elements
			if (!renderer.uiElement)
			{
				sortedEntities[transform.position.z].push_back(entity);
				layersToDraw.insert(transform.position.z);
			}
			else
			{
				sortedUIElements[transform.position.z].push_back(entity);
				uiLayersToDraw.insert(transform.position.z);
			}
		}

		//Draw all sprites and tilemap by layer
		for (const float& layer : layersToDraw)
		{
			if (tilemap)
				tilemap->draw(layer);

			//Bind the right VAO after tilemap
			glBindVertexArray(VAO);

			//Draw entities for this layer
			for (const ecs::Entity& entity : sortedEntities[layer])
			{
				DrawEntity(entity, cam);
			}
		}

		//Draw all UI elements by layer
		glDisable(GL_DEPTH_BUFFER_BIT);
		for (const float& layer : uiLayersToDraw)
		{
			//Bind the right VAO after tilemap
			glBindVertexArray(VAO);

			//Draw entities for this layer
			for (const ecs::Entity& entity : sortedUIElements[layer])
			{
				DrawEntity(entity, cam);
			}
		}
		glEnable(GL_DEPTH_BUFFER_BIT);

		//Unbind vertex array
		glBindVertexArray(0);
	}

	///Draw an entity to the screen
	void SpriteRenderSystem::DrawEntity(ecs::Entity entity, Camera* cam)
	{
		//Get relevant components
		SpriteRenderer& sprite = ecs::GetComponent<SpriteRenderer>(entity);

		if (!sprite.enabled)
			return;

		//If a shader has been specified for this sprite use it, else use the default
		Shader* shader = defaultShader;
		if (sprite.shader)
			shader = sprite.shader;
		shader->use();

		//Create the model matrix
		glm::mat4 model = TransformSystem::GetGlobalTransformMatrix(entity);

		//Give the shader the model matrix
		unsigned int modelLoc = glGetUniformLocation(shader->ID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//Get the view and projection locations
		unsigned int projLoc = glGetUniformLocation(shader->ID, "projection");
		unsigned int viewLoc = glGetUniformLocation(shader->ID, "view");

		if (!sprite.uiElement)
		{
			//Give the shader the camera's view matrix
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(cam->GetViewMatrix()));

			//Give the shader the camera's projection matrix
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(cam->GetProjectionMatrix()));
		}
		else
		{
			//Clear the depth buffer to always draw UI elements on top
			glClear(GL_DEPTH_BUFFER_BIT);

			//Give the shader a constant view matrix
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.f)));

			//Give the shader a constant projection matrix
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.f)));
		}

		//Bind the texture
		glActiveTexture(GL_TEXTURE0);
		if (sprite.texture)
			sprite.texture->Use();

		//Draw the sprite
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//Unbind the texture
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	///Set the screens clear color to given rgb
	void SpriteRenderSystem::SetBackgroundColor(float r, float g, float b)
	{
		glClearColor(r / 255, g / 255, b / 255, 1.0f);
	}

	///Set a tilemap to render
	void SpriteRenderSystem::SetTilemap(Tilemap* map)
	{
		tilemap = map;
	}
	//Remove a tilemap from rendering
	void SpriteRenderSystem::RemoveTilemap()
	{
		tilemap = nullptr;
	}
}