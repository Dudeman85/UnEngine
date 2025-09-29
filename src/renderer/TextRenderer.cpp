#include "renderer/TextRenderer.h"

namespace une::renderer
{
	//Initialize the shaders
	void TextRenderSystem::Init()
	{
		shader = new Shader(
		R"(
			#version 330 core
			layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
			out vec2 TexCoords;
			uniform mat4 projection;
			uniform mat4 view;
			uniform mat4 model;

			void main()
			{
				gl_Position = projection * view * model * vec4(vertex.xy, 0.0, 1.0);
				TexCoords = vertex.zw;
			}
			)",
		R"(
			#version 330 core
			in vec2 TexCoords;
			out vec4 color;
			uniform sampler2D text;
			uniform vec4 textColor;

			void main()
			{
				vec4 fragColor = textColor * vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
				if(fragColor.a <= 0.02)
					discard;
				color = fragColor;
			}
			)",false);
	}

	//Sorts the sprites into their draw layers
	void TextRenderSystem::Prepass()
	{
		opaqueWorldEntities.clear();
		transparentWorldEntities.clear();
		opaqueUIEntities.clear();
		transparentUIEntities.clear();

		//Sort all entities into their draw orders
		for (ecs::Entity entity : entities)
		{
			TextRenderer& text = ecs::GetComponent<TextRenderer>(entity);
			Color srgb = text.color.AsSRGB();
			if (!text.enabled)
				continue;
			if (text.uiElement)
			{
				if (srgb.a > 0.02 && srgb.a < 0.98)
					transparentUIEntities.push_back({entity, DrawEntity});
				else
					opaqueUIEntities.push_back(entity);
			}
			else
			{
				if (srgb.a > 0.02 && srgb.a < 0.98)
					transparentWorldEntities.push_back({entity, DrawEntity});
				else
					opaqueWorldEntities.push_back(entity);
			}
		}
	}

	//Draws all entities in the opaqueWorldEntities list
	void TextRenderSystem::DrawOpaqueWorldEntities(Camera *cam)
	{
		for (ecs::Entity entity : opaqueWorldEntities)
		{
			DrawEntity(entity, cam);
		}
	}

	//Draws all entities in the opaqueUIEntities list, expects depth buffer to be reset
	void TextRenderSystem::DrawOpaqueUIEntities(Camera* cam)
	{
		for (ecs::Entity entity : opaqueUIEntities)
		{
			DrawEntity(entity, cam);
		}
	}

	//Draw a sprite to the screen, expects bound VAO
	void TextRenderSystem::DrawEntity(ecs::Entity entity, Camera* cam)
	{
		TextRenderer& textRenderer = ecs::GetComponent<TextRenderer>(entity);

		if (!textRenderer.font)
		{
			printf("ERROR: No font given!");
			return;;
		}

		shader->Use();

		//Create the model matrix, this is the same for each mesh so it only needs to be done once
		glm::mat4 model = TransformSystem::GetGlobalTransformMatrix(entity);

		//Give the shader the model matrix
		int modelLoc = glGetUniformLocation(shader->ID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//Get the view and projection locations
		int viewLoc = glGetUniformLocation(shader->ID, "view");
		int projLoc = glGetUniformLocation(shader->ID, "projection");

		if (textRenderer.uiElement)
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

		Color srgb = textRenderer.color.AsSRGB();
		glUniform4f(glGetUniformLocation(shader->ID, "textColor"), srgb.r, srgb.g, srgb.b, srgb.a);

		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(textRenderer.font->VAO);

		float x = 0;
		// Renders text one letter at a time
		std::string::const_iterator c;
		for (c = textRenderer.text.begin(); c != textRenderer.text.end(); ++c)
		{
			Character ch = textRenderer.font->characters[*c];

			float xpos = x + ch.Bearing.x;
			float ypos = ch.Size.y - ch.Bearing.y;

			float w = ch.Size.x;
			float h = ch.Size.y;

			float vertices[6][4] = {
				{xpos, ypos + h, 0.0f, 0.0f},
				{xpos, ypos, 0.0f, 1.0f},
				{xpos + w, ypos, 1.0f, 1.0f},

				{xpos, ypos + h, 0.0f, 0.0f},
				{xpos + w, ypos, 1.0f, 1.0f},
				{xpos + w, ypos + h, 1.0f, 0.0f}
			};

			glBindTexture(GL_TEXTURE_2D, ch.TextureID);
			glBindBuffer(GL_ARRAY_BUFFER, textRenderer.font->VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			x += ch.Advance >> 6;
		}

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	const std::vector<Renderable>& TextRenderSystem::GetTransparentWorldEntities()
	{
		return transparentWorldEntities;
	}
	const std::vector<Renderable>& TextRenderSystem::GetTransparentUIEntities()
	{
		return transparentUIEntities;
	}

	Shader* TextRenderSystem::shader = nullptr;
}