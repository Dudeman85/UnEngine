#include "renderer/TextRenderer.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace une::renderer
{
	TextRenderSystem::~TextRenderSystem()
	{
		FT_Done_FreeType(ftLib);
	}

	//Initialize the shaders and freetype
	void TextRenderSystem::Init()
	{
		if (FT_Init_FreeType(&ftLib))
		{
			std::cout << "Failed to initialize FreeType library" << std::endl;
		}

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
		transparentWorldEntities.clear();
		transparentUIEntities.clear();

		//Sort all entities into their draw orders
		for (ecs::Entity entity : entities)
		{
			TextRenderer& text = ecs::GetComponent<TextRenderer>(entity);
			if (!text.enabled)
				continue;
			//Text is always sorted because it is anti-aliased
			if (text.uiElement)
				transparentUIEntities.push_back({entity, DrawEntity});
			else
				transparentWorldEntities.push_back({entity, DrawEntity});
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
			//Size of the character is determined only by the text renderer size
			float size = (float)textRenderer.size / (float)textRenderer.font->GetResolution();

			float xpos = x + ch.bearing.x * size;
			float ypos = -(ch.size.y - ch.bearing.y) * size;
			float w = ch.size.x * size;
			float h = ch.size.y * size;

			float vertices[6][4] = {
				{xpos, ypos + h, 0.0f, 0.0f},
				{xpos, ypos, 0.0f, 1.0f},
				{xpos + w, ypos, 1.0f, 1.0f},

				{xpos, ypos + h, 0.0f, 0.0f},
				{xpos + w, ypos, 1.0f, 1.0f},
				{xpos + w, ypos + h, 1.0f, 0.0f}
			};

			glBindTexture(GL_TEXTURE_2D, ch.textureID);
			glBindBuffer(GL_ARRAY_BUFFER, textRenderer.font->VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			x += (ch.advance >> 6) * size;
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
	FT_Library TextRenderSystem::ftLib;
}