#pragma once

#include <string>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <freetype/freetype.h>

namespace une
{
	//Class to create and store data for the font
	class Font
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

		//Load a font from file with a set dpi resolution
		Font(const std::string& path, unsigned short resolution);
		~Font();

		int GetResolution() const;

		bool Valid();

		unsigned int VAO, VBO;
		std::string name;
		std::unordered_map<char, Character> characters;
	private:
		void LoadCharacters(FT_Face face);

		int resolution;
	};
}