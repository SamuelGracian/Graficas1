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
    Color green = { 51,255,91,255 };

    Image img;

    img.Create(100, 100, 32);

    //img.Drawtriangle(10, 10, 50, 80, 90, 20, red);

	//img.DrawtriangleV3(23,40,65,12,32,14, red);

    img.DrawtriangleV3(25, 20, 12, 10, 32, 54, green);

    img.encode("triangle14.bmp");

    return 0;
}