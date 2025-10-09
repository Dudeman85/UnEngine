#include "renderer/SpriteRenderer.h"

#include <vector>
#include <functional>

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Debug.h"
#include "ECS.h"
#include "Transform.h"
#include "renderer/gl/Shader.h"
#include "renderer/gl/Texture.h"
#include "renderer/gl/Camera.h"

namespace une::renderer
{
	SpriteRenderSystem::~SpriteRenderSystem()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteVertexArrays(1, &VBO);
	}

	//Initialize the shaders and shared buffers
	void SpriteRenderSystem::Init()
	{
		//Create the default sprite shader
		defaultShader = new Shader(
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
			out vec4 FragColor;
			in vec2 TexCoord;
			uniform sampler2D texture1;
			void main()
			{
				vec4 texColor = texture(texture1, TexCoord);
				if(texColor.a <= 0.02)
					discard;
				FragColor = texColor;
			}
			)", false);

		//Rectangle vertices start at top left and go clockwise to bottom left
		const float vertices[] = {
			//Positions		  Texture Coords
			-1.f,  1.f, 0.0f, 0.0f, 1.0f, // top left
			 1.f,  1.f, 0.0f, 1.0f, 1.0f, // top right
			-1.f, -1.f, 0.0f, 0.0f, 0.0f, // bottom left
			 1.f, -1.f, 0.0f, 1.0f, 0.0f, // bottom right
		};

		//Make the Vertex Array Object, Vertex Buffer Object, and Element Buffer Object
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		//Bind the Vertex Array Object
		glBindVertexArray(VAO);

		//Bind the Vertex Bufer Object and set vertices
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		//Configure Vertex attribute at location 0 aka position
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		//Configure Vertex attribute at location 1 aka texture coords
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		//Unbind all buffers and arrays
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	//Sorts the sprites into their draw layers
	void SpriteRenderSystem::Prepass()
	{
		opaqueWorldEntities.clear();
		transparentWorldEntities.clear();
		opaqueUIEntities.clear();
		transparentUIEntities.clear();

		//Sort all entities into their draw orders
		for (ecs::Entity entity : entities)
		{
			SpriteRenderer& sprite = ecs::GetComponent<SpriteRenderer>(entity);
            Vector3 pos = TransformSystem::GetGlobalTransform(entity).position;

			if (!sprite.enabled)
				continue;
			if (sprite.uiElement)
			{
				if (sprite.texture->isSemiTransparent)
					transparentUIEntities.push_back({entity, pos, DrawRenderable});
				else
					opaqueUIEntities.push_back(entity);
			}
			else
			{
				if (sprite.texture->isSemiTransparent)
					transparentWorldEntities.push_back({entity, pos, DrawRenderable});
				else
					opaqueWorldEntities.push_back(entity);
			}
		}
	}

	//Draws all entities in the opaqueWorldEntities list
	void SpriteRenderSystem::DrawOpaqueWorldEntities(Camera *cam)
	{
		for (ecs::Entity entity : opaqueWorldEntities)
		{
			DrawEntity(entity, cam);
		}
	}

	//Draws all entities in the opaqueUIEntities list, expects depth buffer to be reset
	void SpriteRenderSystem::DrawOpaqueUIEntities(Camera* cam)
	{
		for (ecs::Entity entity : opaqueUIEntities)
		{
			DrawEntity(entity, cam);
		}
	}

	//Static version of DrawEntity for renderer
	void SpriteRenderSystem::DrawRenderable(const Renderable& r, Camera* cam)
	{
		ecs::GetSystem<SpriteRenderSystem>()->DrawEntity(r.entity, cam);
	}

	//Draw a sprite to the screen, expects bound VAO
	void SpriteRenderSystem::DrawEntity(ecs::Entity entity, Camera* cam)
	{
		//Get relevant components
		SpriteRenderer& sprite = ecs::GetComponent<SpriteRenderer>(entity);

		if (!sprite.texture)
		{
			debug::LogWarning("No texture given for SpriteRenderer of entity " + std::to_string(entity));
			return;
		}

		//If a shader has been specified for this sprite use it, else use the default
		Shader* shader = defaultShader;
		if (sprite.shader)
			shader = sprite.shader;
		shader->Use();

		//Create the model matrix
		glm::mat4 model = TransformSystem::GetGlobalTransformMatrix(entity);

		//Give the shader the model matrix
		int modelLoc = glGetUniformLocation(shader->ID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//Get the view and projection locations
		int viewLoc = glGetUniformLocation(shader->ID, "view");
		int projLoc = glGetUniformLocation(shader->ID, "projection");

		if (sprite.uiElement)
		{
			//Render UI elements independent of camera's view and projection
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.f)));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.f)));
		}
		else
		{
			//Render World entities based on camera's view and projection
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(cam->GetViewMatrix()));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(cam->GetProjectionMatrix()));
		}

		//Bind the resources
		glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE0);
		sprite.texture->Use();

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		//Unbind the resources
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	const std::vector<Renderable>& SpriteRenderSystem::GetTransparentWorldEntities()
	{
		return transparentWorldEntities;
	}
	const std::vector<Renderable>& SpriteRenderSystem::GetTransparentUIEntities()
	{
		return transparentUIEntities;
	}
}