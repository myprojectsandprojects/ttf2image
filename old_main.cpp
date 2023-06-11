#include <stdio.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

int main()
{

	unsigned char ttf_buffer[1<<20];
	unsigned char temp_bitmap[512*512];
	stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
	fread(ttf_buffer, 1, 1<<20, fopen("/home/eero/all/no-gtk-text-editor/ttf2image/novem___.ttf", "rb"));
   stbtt_BakeFontBitmap(ttf_buffer,0, 32.0, temp_bitmap,512,512, 32,96, cdata); // no guarantee this fits!
   // can free ttf_buffer at this point

	unsigned char *FontFileContents = ttf_buffer;
	stbtt_fontinfo Font;
	stbtt_InitFont(&Font, FontFileContents, stbtt_GetFontOffsetForIndex(FontFileContents, 0));
	int ascent; int descent; int lineGap;
	stbtt_GetFontVMetrics(&Font, &ascent, &descent, &lineGap);
	float Scale = stbtt_ScaleForPixelHeight(&Font, 32.0);
	printf("ascent: %f, descent: %f, line gap: %f\n", ascent*Scale, descent*Scale, lineGap*Scale);
	int X0, Y0, X1, Y1; // relative to baseline, so seems to be: 0 -- lower left, 1 -- upper right
	stbtt_GetFontBoundingBox(&Font, &X0, &Y0, &X1, &Y1);
	printf("x0: %f, y0: %f, x1: %f, y1: %f\n", Scale*X0, Scale*Y0, Scale*X1, Scale*Y1);

//	for(int i = 0; i < 96; ++i)
//	{
//		printf("%d [%c]: x0: %d, x1: %d, y0: %d, y1: %d, xoff: %.1f, yoff: %.1f, xadvance: %.1f\n",
//			i, (i + 32), cdata[i].x0, cdata[i].x1, cdata[i].y0, cdata[i].y1, cdata[i].xoff, cdata[i].yoff, cdata[i].xadvance);
//	}

////	unsigned char Data[] =
////	{
////	0xff, 0xff,
////	0x00, 0x00,
////	0x88, 0x88
////	};
////	int Width = 2;
////	int Height = 3;
////	int Comp = 1; // 1=Y, 2=YA, 3=RGB, 4=RGBA. (Y is monochrome color.)
////	int StrideInBytes = 2;
//	int Width = 512;
//	int Height = 512;
//	int Comp = 1;
//	int StrideInBytes = 512;
//	unsigned char *Data = temp_bitmap;
//	int Result = stbi_write_png("test.png", Width, Height, Comp, (const void *)Data, StrideInBytes);
//	if(Result)
//	{
//		printf("SUCCESS\n");
//	}
//	else
//	{
//		printf("FAILURE\n");
//	}
}


