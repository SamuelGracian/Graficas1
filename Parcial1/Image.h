#pragma once

struct Color 
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};

enum RegionCode
{
  INSIDE = 0,
  LEFT	 = 1,
  RIGHT  = 1 << 1,
  BOTTOM = 1 << 2,
  TOP		 = 1 << 3
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

	void bitBltAlpha(Image & src, int X = 0, int Y = 0, int srcIniX = 0, int srcIniY = 0);

	int computeRegionCode(int x, int y, int xMin, int xMax, int yMin, int yMax);

	bool clipLine(int& x1, int& y1, int& x2, int& y2);

	void BresenhamLine(int x1, int y1, int x2, int y2, const Color& color);

	/// <summary>
	/// Draws a triangle using the Bresenham algorithm
	/// </summary>
	/// <param name="x1"></param> corner 1 in X
	/// <param name="y1"></param> corner 1 in Y
	/// <param name="x2"></param> corner 2 in X
	/// <param name="y2"></param> corner 2 in Y
	/// <param name="x3"></param> corner 3 in X
	/// <param name="y3"></param> corner 3 in Y
	/// <param name="color"></param> color of the triangle
	void Drawtriangle(int x1, int y1, int x2, int y2, int x3, int y3, const Color& color);
	
	/// <summary>
	/// Draws a triangle filled with color using the Bresenham algorithm, and colors it but leaves the edges uncolored
	/// </summary>
	/// <param name="x1"></param>
	/// <param name="y1"></param>
	/// <param name="x2"></param>
	/// <param name="y2"></param>
	/// <param name="x3"></param>
	/// <param name="y3"></param>
	/// <param name="color"></param>
	void DrawtriangleV2(int x1, int y1, int x2, int y2, int x3, int y3, const Color& color);

	/// <summary>
	/// Draw a triangle filled with color using the Bresenham algorithm and colors it
	/// </summary>
	/// <param name="x1"></param>
	/// <param name="y1"></param>
	/// <param name="x2"></param>
	/// <param name="y2"></param>
	/// <param name="x3"></param>
	/// <param name="y3"></param>
	/// <param name="color"></param>
	void DrawtriangleV3(int x1, int y1, int x2, int y2, int x3, int y3, const Color& color);

	/// <summary>
	/// By using the Bresenham algorithm, fills the triangle with the color
	/// </summary>
	/// <param name="x1"></param>
	/// <param name="y1"></param>
	/// <param name="x2"></param>
	/// <param name="y2"></param>
	/// <param name="x3"></param>
	/// <param name="y3"></param>
	/// <param name="color"></param>
	void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, const Color& color);

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
