#include "renderer/gl/Texture.h"

#include <iostream>

#include <stb_image.h>

namespace engine
{
    Texture::Texture(int sx, int sy, const std::vector<std::uint16_t>& data)
	{
		//Generate and bind texture
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16UI, sx, sy, 0, GL_RG_INTEGER, GL_UNSIGNED_SHORT, &data[0]);
		float aniso = 0.0f;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	///Load a texture from path
	Texture::Texture(std::string path, unsigned int filteringType, bool flip)
	{
		//Flip the image when loading into an OpenGL texture
		stbi_set_flip_vertically_on_load(flip);

		int width, height, nrChannels;
		unsigned char* imageData = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

		if (imageData)
		{
			//Set the OpenGL texture format to include alpha if appropriate
			GLint colorFormat;
			if (nrChannels == 4)
			{
				colorFormat = GL_RGBA;
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
		else
		{
			std::cout << "Error loading texture from " << path << std::endl;
		}
	}

	//Create a texture from an image
	Texture::Texture(Image image, unsigned int filteringType)
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

	Texture::~Texture()
	{
		glDeleteTextures(1, &id);
	}

	///Sets the OpenGL sampling type when up and downscaling the texture. Ex. GL_NEAREST, GL_LINEAR, etc.
	void Texture::SetScalingFilter(unsigned int type)
	{
		glBindTexture(GL_TEXTURE_2D, id);

		//Set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, type);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, type);

		//Unbind texture
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	///Get this textures OpenGL ID
	unsigned int Texture::ID()
	{
		return id;
	}

	///Use this texture to draw the next sprite
	void Texture::Use()
	{
		glBindTexture(GL_TEXTURE_2D, id);
	}
}