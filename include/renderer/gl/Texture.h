#pragma once

#include <string>
#include <vector>

#include <glad/gl.h>

#include "Image.h"

namespace engine
{
	///Abstraction class for OpenGL textures
	class Texture
	{
	public:
		Texture(const Texture&) = delete;
		Texture() = delete;

		//Load a texture from an array of pixels
		Texture(int sx, int sy, const std::vector<std::uint16_t>& data);
		//Load a texture from path
		explicit Texture(std::string path, unsigned int filteringType = GL_NEAREST, bool flip = true);
		//Make a texture from an image
		explicit Texture(Image image, unsigned int filteringType = GL_NEAREST);

		~Texture();

		///Sets the OpenGL sampling type when up and downscaling the texture. Ex. GL_NEAREST, GL_LINEAR, etc.
		void SetScalingFilter(unsigned int type);

		///Get this textures OpenGL ID
		unsigned int ID();

		///Use this texture to draw the next sprite
		void Use();

		std::string path;
		std::string type;
	private:
		unsigned int id = 0;
	};
}