/// <summary>
/// This file is only the main, there won't be any code here.
/// </summary>
/* INCLUDES */
#include <iostream>

#include "Image.h"
/* NAMESPACE */
using namespace std;


/* POINTERS */

/* MAIN */
int main()
{
	Color red = { 255, 0, 0, 255 };

    Image img;

    img.Create(100, 100, 24);

    img.encode("triangle2.bmp");

    img.Drawtriangle(10, 10, 50, 80, 90, 20, red);

	img.DrawtriangleV2(10, 10, 50, 80, 90, 20, red);

    img.encode("triangle.bmp");

    return 0;
}