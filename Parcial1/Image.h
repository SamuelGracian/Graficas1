#pragma once

struct Color 
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};

class Image
{
public:
	Image() = default;
	~Image() = default;

	void Create(int width, int height, int bpp);

	//Returns the width of the image
	int getWidth() const;

	//Returns the height of the image
	int getHeight() const;

	//Returns the number of bytes per row
	int getPitch() const;

	//Returns the number of bytes per pixel
	int getBpp() const;

	void decode(const char* filename);

	void encode(const char* filename);

	Color getPixel(unsigned int X, unsigned int Y ) ;

	void setPixel(unsigned int X, unsigned int Y, const Color& color);

	void clearColor(const Color& color);

	void bitBlt(Image& dest, int X, int Y, int width, int height, int destX, int destY);

	void biBltV2(Image& src, int x = 0, int y = 0);

	void bitBltAlpha(Image & src, int X = 0, int Y = 0, int srcIniX = 0, int srcIniY);

	void BresenhamLine(int x1, int y1, int x2, int y2, const Color& color);

protected:
	/*Image resolution*/
	unsigned int m_width = 0; //width
	unsigned int m_height = 0; //Height
	unsigned int m_bpp = 0; //bits per pixel

	unsigned char* m_pixels = nullptr;

	//clipping de lienas
	const int INSIDE = 0; // 0000
	const int LEFT = 1;   // 0001
	const int RIGHT = 2;  // 0010
	const int BOTTOM = 4; // 0100
	const int TOP = 8;    // 1000
};

