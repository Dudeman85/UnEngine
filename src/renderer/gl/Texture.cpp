#include "renderer/gl/Texture.h"

#include "stb_image.h"

#include "debug/Logging.h"
#include "renderer/gl/Window.h"

namespace une
{
	Texture::Texture(GLuint id, Vector2Int size)
	{
		this->id = id;
		this->size = size;
		semiTransparent = true;
	}

	//Load pixel data from disk with stbimage
	bool Texture::Load(const std::string& path, bool editable, bool flip)
	{
		stbi_set_flip_vertically_on_load(flip);

		this->path = path;
		int width, height, nrChannels;
		imageData = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
		if (!imageData)
		{
			debug::LogError("Error loading texture from " + path);
			stbi_image_free(imageData);
			return false;
		}

		//Set the OpenGL texture format to include alpha if appropriate
		if (nrChannels == 4)
		{
			colorFormat = GL_RGBA;
			//Check for semi-transparency
			for (int i = 3; i < width * height; i+=4)
			{
				if (imageData[i] > 5 && imageData[i] < 250)
				{
					semiTransparent = true;
					break;
				}
			}
		}
		else if (nrChannels == 3)
		{
			colorFormat = GL_RGB;
		}
		else
		{
			colorFormat = GL_RED;
		}

		size = {width, height};
		this->editable = editable;
		debug::LogSpam("Successfully loaded texture " + path);
		return true;
	}

	//Make OpenGl Texture using loaded pixel data
	bool Texture::SetupGLResources()
	{
		if (!SetupGLResources(imageData, size, colorFormat))
		{
			stbi_image_free(imageData);
			editable = false;
			return false;
		}

		//Image data is no longer needed if not editable
		if (!editable)
		{
			stbi_image_free(imageData);
		}

		return true;
	}

	//Make OpenGL Texture using arbitrary pixel data
	bool Texture::SetupGLResources(const uint8_t* data, Vector2Int size, GLint colorFormat)
	{
		if (!data)
		{
			debug::LogError("Failed to generate texture: No pixel data loaded");
			return false;
		}

		//Generate and bind texture
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		if (!id)
		{
			debug::LogError("Failed to generate GL texture from image: Make sure to only call this from the main thread");
			debug::LogGLError();
			return false;
		}

		//Generate the texture using the image data
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, colorFormat, size.x, size.y, 0, colorFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);

		this->size = size;
		debug::LogSpam("Successfully setup gl resources for texture " + path);
		return true;
	}

	Texture::~Texture()
	{
		if (mainWindow && id)
		{
			glDeleteTextures(1, &id);
		}
		if (editable)
		{
			stbi_image_free(imageData);
		}
	}

	//Sets the OpenGL sampling type when up and downscaling the texture. Ex. GL_NEAREST, GL_LINEAR, etc.
	void Texture::SetScalingFilter(GLuint filter)
	{
		glBindTexture(GL_TEXTURE_2D, id);

		//Set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

		//Unbind texture
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
