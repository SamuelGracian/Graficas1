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

    img.Create(100, 100, 32);

    //img.Drawtriangle(10, 10, 50, 80, 90, 20, red);

	img.DrawtriangleV2(30, 30, 90, 80, 24, 7, red);

    img.encode("triangle10.bmp");

    return 0;
}