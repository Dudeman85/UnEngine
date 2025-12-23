#pragma once

#include <string>

#ifndef __gl_h_
#include "glad/gl.h"
#endif

#include "Vector.h"
#include "Image.h"

namespace une
{
	//Abstraction class for OpenGL textures
	class Texture
	{
	public:
		Texture(const Texture&) = delete;
		Texture() = delete;
		//Make a texture from an existing opengl texture id
		explicit Texture(unsigned int id);
		//Load a texture from path
		explicit Texture(const std::string& path, unsigned int filteringType = GL_NEAREST, bool flip = true);
		//Make a texture from an image
		explicit Texture(const Image& image, unsigned int filteringType = GL_NEAREST);

		Texture(Texture&& other) noexcept;

		~Texture();

		//Sets the OpenGL sampling filter when up and downscaling the texture. Ex. GL_NEAREST, GL_LINEAR, etc.
		void SetScalingFilter(unsigned int filter);

		//Get this textures OpenGL ID
		unsigned int ID();

		bool Valid();

		//Use this texture on the next draw call
		void Use();

		Vector2Int size;
		bool isSemiTransparent = false;
		std::string path;
		//Texture type primarily for 3D models Ex. texture_diffuse or texture_specular
		std::string type;
	private:
		unsigned int id = 0;
	};
}