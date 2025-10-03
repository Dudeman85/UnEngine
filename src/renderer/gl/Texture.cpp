#include "renderer/gl/Texture.h"

#include <iostream>

#include <stb_image.h>

namespace une
{
	//Make a texture from an existing opengl texture id
	Texture::Texture(unsigned int id)
	{
		this->id = id;
	}

	//Load a texture from path
	Texture::Texture(const std::string& path, unsigned int filteringType, bool flip)
	{
		this->path = path;

		//Flip the image when loading into an OpenGL texture
		stbi_set_flip_vertically_on_load(flip);

		int width, height, nrChannels;
		unsigned char* imageData = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

		if (!imageData)
		{
			std::cout << "Error loading texture from " << path << std::endl;
			return;
		}

		//Set the OpenGL texture format to include alpha if appropriate
		GLint colorFormat;
		if (nrChannels == 4)
		{
			colorFormat = GL_RGBA;
			//Check for semi-transparency
			for (int i = 3; i < width * height; i+=4)
			{
				if (imageData[i] > 5 && imageData[i] < 250)
				{
					isSemiTransparent = true;
					break;
				}
			}
		}
		else if (nrChannels == 3)
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			colorFormat = GL_RGB;
		}
		else
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			colorFormat = GL_RED;
		}

		//Generate and bind texture
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		//Set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filteringType);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filteringType);

		//Generate the texture using the image data
		glTexImage2D(GL_TEXTURE_2D, 0, colorFormat, width, height, 0, colorFormat, GL_UNSIGNED_BYTE, imageData);
		glGenerateMipmap(GL_TEXTURE_2D);

		//Unbind texture
		glBindTexture(GL_TEXTURE_2D, 0);

		//Image data is no longer needed
		stbi_image_free(imageData);
	}

	//Create a texture from an image
	Texture::Texture(const Image& image, unsigned int filteringType)
	{
		//Convert the image to a 1D char array for OpenGL
		unsigned char* imageData = new unsigned char[image.width * image.height * 4];
		int i = 0;
		//Make sure to flip the vertical for OpenGL
		for (int y = image.height - 1; y >= 0; y--)
		{
			for (int x = 0; x < image.width; x++)
			{
				imageData[i++] = image[x][y].r;
				imageData[i++] = image[x][y].g;
				imageData[i++] = image[x][y].b;
				imageData[i++] = image[x][y].a;
				if (image[x][y].a > 5 && image[x][y].a < 250)
					isSemiTransparent = true;
			}
		}

		//Generate and bind texture
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		//Set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filteringType);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filteringType);

		//Generate the texture using the image data
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
		glGenerateMipmap(GL_TEXTURE_2D);

		//Unbind texture
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	Texture::Texture(Texture&& other) noexcept
    {
    	isSemiTransparent = other.isSemiTransparent;
	    path = other.path;
    	type = other.type;
    	id = other.id;
    }

	Texture::~Texture()
	{
		glDeleteTextures(1, &id);
	}

	//Sets the OpenGL sampling type when up and downscaling the texture. Ex. GL_NEAREST, GL_LINEAR, etc.
	void Texture::SetScalingFilter(unsigned int filter)
	{
		glBindTexture(GL_TEXTURE_2D, id);

		//Set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

		//Unbind texture
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	//Get this textures OpenGL ID
	unsigned int Texture::ID()
	{
		return id;
	}

	//Use this texture on the next draw call
	void Texture::Use()
	{
		glBindTexture(GL_TEXTURE_2D, id);
	}
}