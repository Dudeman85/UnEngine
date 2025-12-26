#pragma once

#include <string>
#include <unordered_map>

#include "glm/glm.hpp"
#include "freetype/freetype.h"

#include "utils/Resource.h"

namespace une
{
	//Class to create and store data for the font
	class Font : public resources::Resource
	{
	public:
		//Struct to store data about the characters
		struct Character
		{
			//OpenGL texture id of this character
			unsigned int textureID;
			//The size of the characters in pixels
			glm::ivec2 size;
			//The offset of this character from its origin
			glm::ivec2 bearing;
			//The distance from the origin of this character to the next
			unsigned int advance;
		};

		Font() = default;
		~Font() override;

		bool Load(const std::string& path, FT_UInt resolution = 64);
		bool SetupGLResources() override;

		bool Valid() const override {return VAO != 0;};
		int GetResolution() const {return resolution;};

		unsigned int VAO, VBO;
		//Font family
		std::string name;
		std::unordered_map<char, Character> characters;

	private:
		int resolution;
		FT_Face face;
	};
}
