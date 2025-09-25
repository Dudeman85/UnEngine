#include "Image.h"

#include <iostream>
#include <cassert>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace une
{
	///Load the image data from a path
	Image::Image(const std::string& path)
	{
		//Dont flip the image when loading to an Image
		stbi_set_flip_vertically_on_load(false);
		//Load image
		unsigned char* imageData = stbi_load((path).c_str(), &width, &height, &channels, 4);

		//If the image is loaded successfully
		if (imageData)
		{
			//Resize the pixmap vector to be able to insert just by index
			pixmap.resize(width);
			for (size_t i = 0; i < width; i++)
				pixmap[i].resize(height);

			int i = 0;
			//For each row and column
			for (size_t y = 0; y < height; y++)
			{
				for (size_t x = 0; x < width; x++)
				{
					//Get the rgba values and put them in a nice to use 2D vector of Pixels
					pixmap[x][y] = Pixel{ imageData[i], imageData[i + 1], imageData[i + 2], imageData[i + 3] };
					i += 4;
				}
			}

			//Image data is no longer needed
			stbi_image_free(imageData);
		}
		else
		{
			std::cout << "Error loading texture from " << path << std::endl;
		}
	}
	///Construct an image from a 2D vector of pixels
	Image::Image(std::vector<std::vector<Pixel>> pixels)
	{
		width = pixels.size();
		height = pixels[0].size();
		channels = 4;
		pixmap = pixels;
	}

	std::vector<Pixel> Image::operator[](int i)
	{
		return pixmap[i];
	}
	const std::vector<Pixel> Image::operator[](int i) const
	{
		return pixmap[i];
	}

	///Returns the data of this imge in stbimage friendly format
	unsigned char* Image::data()
	{
		unsigned char* data = new unsigned char[width * height * 4];
		int i = 0;
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				data[i++] = pixmap[x][y].r;
				data[i++] = pixmap[x][y].g;
				data[i++] = pixmap[x][y].b;
				data[i++] = pixmap[x][y].a;
			}
		}
		return data;
	}

	///Get a subsection of pixels from x1 y1 top-left, to x2, y2 bottom-right (inclusive).
	Image Image::Slice(int x1, int y1, int x2, int y2)
	{
		assert(x1 < x2&& y1 < y2 && "x1 and y1 must be less than x2 and y2!");
		assert(x1 >= 0 && y1 >= 0 && x2 < width&& y2 < height && "Slice must be in bounds of original image!");

		std::vector<std::vector<Pixel>> slice;
		slice.resize(x2 - x1 + 1);
		for (size_t i = 0; i < x2 - x1 + 1; i++)
		{
			slice[i].resize(y2 - y1 + 1);
		}

		//For the region defined by parameters
		int sliceX = 0;
		for (size_t x = x1; x < x2; x++)
		{
			int sliceY = 0;
			for (size_t y = y1; y < y2; y++)
			{
				//Move the pixels from this pixmap to the sliced one
				slice[sliceX][sliceY] = pixmap[x][y];
				sliceY++;
			}
			sliceX++;
		}

		return Image(slice);
	}
}
