#pragma once

#include <string>
#include <vector>

#include <glad/gl.h>

#include "Image.h"

namespace engine
{
	//Abstraction class for OpenGL textures
	class Texture
	{
	public:
		Texture(const Texture&) = delete;
		Texture() = delete;

		//Load a texture from an array of pixels
		Texture(int sx, int sy, const std::vector<std::uint16_t>& data);
		//Load a texture from path
		explicit Texture(const std::string& path, unsigned int filteringType = GL_NEAREST, bool flip = true);
		//Make a texture from an image
		explicit Texture(const Image& image, unsigned int filteringType = GL_NEAREST);

		~Texture();

		//Sets the OpenGL sampling filter when up and downscaling the texture. Ex. GL_NEAREST, GL_LINEAR, etc.
		void SetScalingFilter(unsigned int filter);

		//Get this textures OpenGL ID
		unsigned int ID();

		//Use this texture on the next draw call
		void Use();

		bool isSemiTransparent = false;
		std::string path;
		//Texture type Ex. texture_diffuse or texture_specular
		std::string type;
	private:
		unsigned int id = 0;
	};
}