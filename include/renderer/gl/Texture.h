#pragma once

#include <string>

#ifndef __gl_h_
#include "glad/gl.h"
#endif

#include "Vector.h"
#include "utils/Resource.h"

namespace une
{
	//Abstraction class for OpenGL textures
	class Texture : public resources::Resource
	{
	public:
		Texture() = default;
		//From existing OpenGL texture
		explicit Texture(GLuint id, Vector2Int size);
		~Texture() override;

		//Load pixel data from disk with stbimage
		bool Load(const std::string& path, bool editable = false, bool flip = true);
		//Make OpenGl Texture using loaded pixel data
		bool SetupGLResources() override;
		//Make OpenGL Texture using arbitrary pixel data
		bool SetupGLResources(const uint8_t* data, Vector2Int size, GLint colorFormat);

		//Sets the OpenGL sampling filter when up and downscaling the texture. Ex. GL_NEAREST, GL_LINEAR, etc.
		void SetScalingFilter(GLuint filter);

		//Is the OpenGl texture usable
		bool Valid() const override {return id != 0;};
		//Opengl texture ID
		GLuint ID() const {return id;};
		//Pixel size of the texture image
		Vector2Int Size() const {return size;};
		//Does this texture contain any pixels where a != 0 or 1
		bool SemiTransparent() const {return semiTransparent;};

		//Texture type primarily for 3D models Ex. texture_diffuse or texture_specular
		std::string textureType;

	private:
		GLuint id = 0;
		Vector2Int size;
		bool semiTransparent = false;

		uint8_t* imageData = nullptr;
		GLint colorFormat = 0;
	};
}
