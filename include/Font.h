#pragma once

#include <string>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <freetype/freetype.h>

#include "renderer/gl/Shader.h"

///Struct to store data about the characters
struct Character
{
	///Automatic ID for every character
	unsigned int TextureID;
	///The size of the characters
	glm::ivec2 Size;
	///The rotation of the characters
	glm::ivec2 Bearing;
	unsigned int Advance;
};

namespace une
{
	///Class to create and store data for the font
	class Font
	{
	public:
		/// Constructor
		Font(std::string filepathname, FT_Long face_index, FT_UInt pixel_width, FT_UInt pixel_height);

		// Function for characters resolution.
		void SetResolution(FT_UInt pixel_width, FT_UInt pixel_height);

		// Destructor
		~Font();

		unsigned int VAO, VBO;
		///A map of all the characters
		std::map<GLchar, Character> characters;

	private:
		// VAO & VBO function
		void config();
		// Loading function
		void load();
		
		unsigned int texture;

		FT_Library ft;
		FT_Face face;
	};
}