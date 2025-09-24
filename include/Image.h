#pragma once

#include <vector>
#include <string>

namespace engine
{
	///Struct representing a pixel's Red, Green, Blue, and Alpha values
	struct Pixel
	{
		unsigned char r, g, b, a;
	};

	///Image class basically just a 2d vector of rgba values
	class Image
	{
	public:
		///Load the image data from a path
		explicit Image(const std::string& path);
		///Construct an image from a 2D vector of pixels
		explicit Image(std::vector<std::vector<Pixel>> pixels);

		std::vector<Pixel> operator[](int i);
		const std::vector<Pixel> operator[](int i) const;

		///Returns the data of this imge in stbimage friendly format
		unsigned char* data();

		///Get a subsection of pixels from x1 y1 top-left, to x2, y2 bottom-right (inclusive).
		Image Slice(int x1, int y1, int x2, int y2);

		///the width of the image
		int width;
		///the height of the image
		int height;
	private:
		int channels;
		std::vector<std::vector<Pixel>> pixmap;
	};
}