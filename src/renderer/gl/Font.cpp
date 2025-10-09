#include "renderer/gl/Font.h"

#include "Debug.h"
#include "renderer/TextRenderer.h"

namespace une
{
	Font::Font(const std::string& path, FT_UShort resolution, int faceIndex)
	{
		FT_Face face;
		if (FT_New_Face(renderer::TextRenderSystem::ftLib, path.c_str(), faceIndex, &face))
		{
			debug::LogError("Failed to load font from " + path);
			return;
		}

		FT_Set_Pixel_Sizes(face, 0, resolution);
		this->resolution = face->size->metrics.y_ppem;
		LoadCharacters(face);
	}
	Font::~Font()
	{
		glDeleteBuffers(1, &VAO);
		glDeleteBuffers(1, &VBO);
		for (auto& character : characters)
		{
			glDeleteTextures(1, &character.second.textureID);
		}
	}

	int Font::GetResolution() const
	{
		return resolution;
	}

	void Font::LoadCharacters(FT_Face face)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		for (unsigned char c = 0; c < 128; ++c)
		{
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				debug::LogError("Failed to load character " + std::to_string(c));
				continue;
			}

			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			Character character = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				static_cast<unsigned int>(face->glyph->advance.x)
			};

			characters.insert(std::pair<char, Character>(c, character));
		}

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);

		FT_Done_Face(face);
	}
}
