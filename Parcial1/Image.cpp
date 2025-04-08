#include "Image.h"
#include <fstream>
#include <Windows.h>

using std::fstream;
using std::ios;

#pragma pack (push,2)

struct _bitMapFileHeader
{
	unsigned short bfTpe; //signature
	unsigned long bfSize; //size of the file
	unsigned short bfReserved1; //reserved
	unsigned short bfReserved2;	//reserved
	unsigned long bfOffBits; //offset to the bitmap data
};

#pragma pack (pop)

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

	bmpInfo.CoreHeader.bcSize = sizeof(_bitMapSaveHeader);
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

int Image::computeRegionCode(int x, int y, int xMin, int xMax, int yMin, int yMax)
{
	int code = INSIDE;

  if (x < xMin) { code |= LEFT; }
  else if (x > xMax) { code |= RIGHT; }

  if (y < yMin) { code |= BOTTOM; }
  else if (y > yMax) { code |= TOP; }

  return code;
}

bool Image::clipLine(int& x1, int& y1, int& x2, int& y2)
{
	int xMin = 0;
	int yMin = 0;
	int xMax = m_width;
  int yMax = m_height;

  int code0 = computeRegionCode(x1, y1, xMin, xMax, yMin, yMax);
  int code1 = computeRegionCode(x2, y2, xMin, xMax, yMin, yMax);

	while (true)
	{
		if (!(code0 | code1))
		{
      return true;
		}
    else if (code0 & code1)
    {
      return false;
    }
		else
		{
      int codeOut = code0 ? code0 : code1;

      if (codeOut & TOP)
      {
        x1 += (x2 - x1) * (yMax - y1) / (y2 - y1);
        y1 = yMax;
      }
      else if (codeOut & BOTTOM)
      {
        x1 += (x2 - x1) * (yMin - y1) / (y2 - y1);
        y1 = yMin;
      }
      else if (codeOut & RIGHT)
      {
        y1 += (y2 - y1) * (xMax - x1) / (x2 - x1);
        x1 = xMax;
      }
      else if (codeOut & LEFT)
      {
        y1 += (y2 - y1) * (xMin - x1) / (x2 - x1);
        x1 = xMin;
      }

      if (codeOut == code0)
      {
        code0 = computeRegionCode(x1, y1, xMin, xMax, yMin, yMax);
      }
      else
      {
        code1 = computeRegionCode(x1, y1, xMin, xMax, yMin, yMax);
      }

		}
	}

	return true;
}


    void Image::Drawtriangle(int x1, int y1, int x2, int y2, int x3, int y3, const Color& color)
    {
		// Create edges of the triangle
        int x[3] = { x1, x2, x3 };
        int y[3] = { y1, y2, y3 };

		// Check if the vertices are aligned
        int area = x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2);
        if (area == 0)
        {
			// if the area is 0, the vertices are aligned
            return;
        }

		// Set the triangle as top or bottom
        bool TopTriangle = false;
        bool BottomTriangle = false;

		// Order the vertices by y
        if (y1 < y2 && y1 < y3)
        {
			// y1 is the smallest
            if (y2 < y3)
            {
                // y1 < y2 < y3
                TopTriangle = true;
            }
            else
            {
                // y1 < y3 < y2
                BottomTriangle = true;
            }
        }
        else if (y2 < y1 && y2 < y3)
        {
			// y2 is the smallest
            if (y1 < y3)
            {
                // y2 < y1 < y3
                TopTriangle = true;
            }
            else
            {
                // y2 < y3 < y1
                BottomTriangle = true;
            }
        }
        else
        {
			// y3 is the smallest
            if (y1 < y2)
            {
                // y3 < y1 < y2
                TopTriangle = true;
            }
            else
            {
                // y3 < y2 < y1
                BottomTriangle = true;
            }
        }

		// draw the triangle using the Bresenham algorithm
        BresenhamLine(x1, y1, x2, y2, color);
        BresenhamLine(x2, y2, x3, y3, color);
        BresenhamLine(x3, y3, x1, y1, color);
    }

void Image::DrawtriangleV2(int x1, int y1, int x2, int y2, int x3, int y3, const Color& color)
{
    // Create edges of the triangle
    int x[3] = { x1, x2, x3 };
    int y[3] = { y1, y2, y3 };

    // Check if the vertices are aligned
    int area = x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2);
    if (area == 0)
    {
        // if the area is 0, the vertices are aligned
        return;
    }

    // Set the triangle as top or bottom
    bool TopTriangle = false;
    bool BottomTriangle = false;

    // Order the vertices by y
    if (y1 < y2 && y1 < y3)
    {
        // y1 is the smallest
        if (y2 < y3)
        {
            // y1 < y2 < y3
            TopTriangle = true;
        }
        else
        {
            // y1 < y3 < y2
            BottomTriangle = true;
        }
    }
    else if (y2 < y1 && y2 < y3)
    {
        // y2 is the smallest
        if (y1 < y3)
        {
            // y2 < y1 < y3
            TopTriangle = true;
        }
        else
        {
            // y2 < y3 < y1
            BottomTriangle = true;
        }
    }
    else
    {
        // y3 is the smallest
        if (y1 < y2)
        {
            // y3 < y1 < y2
            TopTriangle = true;
        }
        else
        {
            // y3 < y2 < y1
            BottomTriangle = true;
        }
    }

    // Calculate the midpoint
    int midX = (x1 + x2 + x3) / 3;
    int midY = (y1 + y2 + y3) / 3;

    // Clip the lines to divide the triangle
    int x1_clipped = x1, y1_clipped = y1;
    int x2_clipped = x2, y2_clipped = y2;
    int x3_clipped = x3, y3_clipped = y3;

    clipLine(x1_clipped, y1_clipped, midX, midY);
    clipLine(x2_clipped, y2_clipped, midX, midY);
    clipLine(x3_clipped, y3_clipped, midX, midY);

    // Draw the clipped triangle using the Bresenham algorithm
    BresenhamLine(x1_clipped, y1_clipped, x2_clipped, y2_clipped, color);
    BresenhamLine(x2_clipped, y2_clipped, x3_clipped, y3_clipped, color);
    BresenhamLine(x3_clipped, y3_clipped, x1_clipped, y1_clipped, color);

    // Fill the two resulting triangles
    fillTriangle(x1, y1, x2, y2, midX, midY, color);
    fillTriangle(x2, y2, x3, y3, midX, midY, color);
}


void Image::fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, const Color& color)
{

    if (y1 > y2) { std::swap(x1, x2); std::swap(y1, y2); }
    if (y1 > y3) { std::swap(x1, x3); std::swap(y1, y3); }
    if (y2 > y3) { std::swap(x2, x3); std::swap(y2, y3); }

    auto drawHorizontalLine = [&](int xStart, int xEnd, int y) {
        if (xStart > xEnd) std::swap(xStart, xEnd);
        BresenhamLine(xStart, y, xEnd, y, color);
        };

    auto interpolate = [](int x1, int y1, int x2, int y2, int y) {
        if (y2 == y1) return x1;
        return x1 + (x2 - x1) * (y - y1) / (y2 - y1);
        };


    for (int y = y1; y <= y2; ++y)
    {
        int xStart = interpolate(x1, y1, x3, y3, y);
        int xEnd = interpolate(x1, y1, x2, y2, y);
        drawHorizontalLine(xStart, xEnd, y);
    }

    for (int y = y2; y <= y3; ++y)
    {
        int xStart = interpolate(x1, y1, x3, y3, y);
        int xEnd = interpolate(x2, y2, x3, y3, y);
        drawHorizontalLine(xStart, xEnd, y);
    }
}
