#include "Image.h"

#include <fstream>
#include <Windows.h>

using std::fstream;
using std::ios;

struct _bitMapFileHeader
{
	unsigned short bfTpe; //signature
	unsigned long bfSize; //size of the file
	unsigned short bfReserved1; //reserved
	unsigned short bfReserved2;	//reserved
	unsigned long bfOffBits; //offset to the bitmap data
};

struct _bitMapCoreHeader
{
	unsigned long bcSize; //size of the header
	unsigned long bcWidth; //width of the image
	unsigned long bcHeight; //height of the image
	unsigned short bcPlanes; //number of planes
	unsigned short bcBitCount; //number of bits per pixel
};

struct _bitMapSaveHeader
{
	_bitMapCoreHeader CoreHeader;
	unsigned int biCompression; //compression type
	unsigned int biSizeImage; //size of the image
	long		 biXPelsPerMeter; //horizontal resolution
	long		 biYPelsPerMeter; //vertical resolution
	unsigned long biClrUsed; //number of colors in the color table
	unsigned long biClrImportant; //number of important colors used
};

void Image::Create(int width, int height, int bpp)
{
	m_width = width;
	m_height = height;
	m_bpp = bpp;
	m_pixels = new unsigned char[getPitch() * m_height];
}

int Image::getWidth() const
{
	return m_width;
}

int Image::getHeight() const
{
	return m_height;
}

int Image::getPitch() const
{
	return m_width * m_bpp / 8;
}

int Image::getBpp() const
{
	return m_bpp >> 3;
}

void Image::encode(const char* filename)
{
	fstream imgfile(filename, ios::out | ios::binary);

	if (!imgfile.is_open())
	{
		return;
	}

	_bitMapFileHeader fileHeader;
	_bitMapSaveHeader bmpInfo;

	memset(&fileHeader, 0, sizeof(_bitMapFileHeader));
	memset(&bmpInfo, 0, sizeof(_bitMapSaveHeader));

	int padding = getPitch() % 4;
	int lineMemoryWidth = getPitch();

	if (padding)
	{
		padding = 4 - padding;
		lineMemoryWidth += padding;
	}

	int headerSize = sizeof(_bitMapFileHeader) + sizeof(_bitMapSaveHeader);

	fileHeader.bfTpe = 0x4d42; //BM
	fileHeader.bfSize = headerSize + lineMemoryWidth * m_height;
	fileHeader.bfOffBits = headerSize;

	bmpInfo.CoreHeader.bcSize = sizeof(_bitMapCoreHeader);
	bmpInfo.CoreHeader.bcWidth = m_width;
	bmpInfo.CoreHeader.bcHeight = m_height;
	bmpInfo.CoreHeader.bcPlanes = 1;
	bmpInfo.CoreHeader.bcBitCount = m_bpp;
	bmpInfo.biXPelsPerMeter = 3780;
	bmpInfo.biYPelsPerMeter = 3780;

	imgfile.write(reinterpret_cast<char*>(&fileHeader), sizeof(_bitMapFileHeader));
	imgfile.write(reinterpret_cast<char*>(&bmpInfo), sizeof(_bitMapSaveHeader));

	char paddBuffer[3] = { 0, 0, 0 };

	for (int Line = m_height - 1; Line >= 0; --Line)
	{
		imgfile.write(reinterpret_cast<char*>(&m_pixels[getPitch() * Line]), getPitch());
		if (padding)
		{
			imgfile.write(paddBuffer, padding);
		}
	}
}

void Image::decode(const char* filename)
{
	fstream imgfile(filename, ios::in | ios::binary | ios::ate);

	if (!imgfile.is_open())
	{
		return;
	}

	auto fileSize = imgfile.tellg();

	imgfile.seekg(0, ios::beg);

	_bitMapFileHeader fileHeader;
	imgfile.read(reinterpret_cast<char*>(&fileHeader), sizeof(_bitMapFileHeader));

	if (fileHeader.bfTpe != 0x4d42)
	{
		return;
	}

	_bitMapCoreHeader infoHeader;
	imgfile.read(reinterpret_cast<char*>(&infoHeader), sizeof(_bitMapCoreHeader));
	imgfile.seekg(fileHeader.bfOffBits);

	Create(infoHeader.bcWidth, infoHeader.bcHeight, infoHeader.bcBitCount);

	int padding = getPitch() % 4;
	int lineMemoryWidth = getPitch() + padding;

	for (int Line = m_height - 1; Line >= 0; --Line)
	{
		imgfile.seekg(lineMemoryWidth * Line + fileHeader.bfOffBits);
		imgfile.read(reinterpret_cast<char*> (&m_pixels[getPitch() * (m_height - 1 - Line)]), getPitch());
	}
	int ready = 1;
}

Color Image::getPixel(unsigned int X, unsigned int Y)
{
	Color color;
	int pixelPosition = (getPitch() * Y) + (getBpp() * X);

	color.r = m_pixels[pixelPosition + 2];
	color.g = m_pixels[pixelPosition + 1];
	color.b = m_pixels[pixelPosition + 0];
	color.a = 255;
	return color;
}

void Image::setPixel(unsigned int X, unsigned int Y, const Color& color)
{
	int pixelPosition = (getPitch() * Y) + (getBpp() * X);

	m_pixels[pixelPosition + 2] = color.r;
	m_pixels[pixelPosition + 1] = color.g;
	m_pixels[pixelPosition + 0] = color.b;
}

void Image::clearColor(const Color& color)
{
	for (int i = 0; i < m_width * m_height; ++i)
	{
		m_pixels[i * 3 + 0] = color.b;
		m_pixels[i * 3 + 1] = color.g;
		m_pixels[i * 3 + 2] = color.r;
	}
}

void Image::bitBlt(Image& dest, int X, int Y, int width, int height, int destX, int destY)
{
	//iterate over the row of pixels to copy
	for (int i = 0; i < height; ++i)
	{
		//Iterate over the column of pixels to copy
		for (int j = 0; j < width; ++j)
		{
			//Copy the pixel
			/*
			*Copies the pixel int the posiiton (X + j, Y + i) from the source image to the position (destX + j, destY + i) in the destination image
			* getPixel gets the color of the pixel in the position (X + j, Y + i) from the source image
			* Sets the color of the pixel in the position (destX + j, destY + i) in the destination image
			*/
			dest.setPixel(destX + j, destY + i, getPixel(X + j, Y + i));
		}
	}
}

void Image::BresenhamLine(int x1, int y1, int x2, int y2, const Color& color)
{
	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);
	int sx = (x1 < x2) ? 1 : -1;
	int sy = (y1 < y2) ? 1 : -1;
	int err = dx - dy;

	while (true)
	{
		setPixel(x1, y1, color);

		if (x1 == x2 && y1 == y2)
			break;

		int e2 = 2 * err;
		if (e2 > -dy)
		{
			err -= dy;
			x1 += sx;
		}
		if (e2 < dx)
		{
			err += dx;
			y1 += sy;
		}
	}
}
