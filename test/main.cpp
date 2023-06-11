#include "font.cpp"
#include <stdio.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main()
{
	unsigned char *FontBitmap = uncompress_font();

	int NumComponents = 1;
	int StrideInBytes = BitmapWidth * NumComponents;
	int Result = stbi_write_png("font.png", BitmapWidth, BitmapHeight, NumComponents, (const void *)FontBitmap, StrideInBytes);
	if(!Result)
	{
		printf("FAILURE\n");
	}
}


