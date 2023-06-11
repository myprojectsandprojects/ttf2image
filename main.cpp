
/*
	todo:
	* decent file reading
	* command line arguments (font file, font size)
	* write glyph dimensions into the name of the final png
*/

#include <stdio.h>
#include <limits.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

struct glyph
{
	unsigned char *Bitmap;
	int Width, Height;
	int XOff, YOff;
};

unsigned char *make_font_image(const char *FontFile, double FontHeight, int *Width, int *Height)
{
	unsigned char FileContents[1<<20];
	FILE *FP = fopen(FontFile, "rb");
	size_t NumBytes = fread(FileContents, 1, 1<<20, FP);
//	printf("bytes read: %ld\n", NumBytes);

	stbtt_fontinfo Font;
	if(!stbtt_InitFont(&Font, FileContents, stbtt_GetFontOffsetForIndex(FileContents, 0)))
	{
		fprintf(stderr, "error: stbtt_InitFont()\n");
		return NULL;
	}

	float Scale = stbtt_ScaleForPixelHeight(&Font, FontHeight);

//	int Ascent; int Descent; int LineGap;
//	stbtt_GetFontVMetrics(&Font, &Ascent, &Descent, &LineGap);
//	printf("ascent: %.1f, descent: %.1f, line gap: %.1f\n", Ascent*Scale, Descent*Scale, LineGap*Scale);

	char FirstChar = ' ';
//	char FirstChar = 'a';
	int NumGlyphs = 95;
//	int NumGlyphs = 3;
	glyph Glyphs[NumGlyphs];

	for(int i = 0; i < NumGlyphs; ++i)
	{
		char Ch = FirstChar + i;
		int GlyphWidth, GlyphHeight, XOff, YOff;
		unsigned char *GlyphBitmap = stbtt_GetCodepointBitmap(&Font, Scale, Scale, Ch, &GlyphWidth, &GlyphHeight, &XOff, &YOff);

		Glyphs[i].Bitmap = GlyphBitmap;
		Glyphs[i].Width = GlyphWidth;
		Glyphs[i].Height = GlyphHeight;
		Glyphs[i].XOff = XOff;
		Glyphs[i].YOff = YOff;
//		STBTT_DEF void stbtt_FreeBitmap(unsigned char *bitmap, void *userdata)
	}

	// These are positions relative to the baseline (baseline + HighestEdge)
	// So if y-coordinate is increasing downwards then negative values are above the baseline and positive values are below the baseline.
	int HighestEdge = INT_MAX;
	int LowestEdge = INT_MIN;

	// These are relative to the glyphs x-position
	int LeftestEdge = INT_MAX;
	int RightestEdge = INT_MIN;

	for(int i = 0; i < NumGlyphs; ++i)
	{
//		printf("%c: width: %d, height: %d, x-offset: %d, y-offset: %d\n",
//			FirstChar + i, Glyphs[i].Width, Glyphs[i].Height, Glyphs[i].XOff, Glyphs[i].YOff);

		int UpperEdge = Glyphs[i].YOff;
		int LowerEdge = Glyphs[i].Height + Glyphs[i].YOff;
		int LeftEdge = Glyphs[i].XOff;
		int RightEdge = Glyphs[i].XOff + Glyphs[i].Width;

		if(UpperEdge < HighestEdge) HighestEdge = UpperEdge;
		if(LowerEdge > LowestEdge) LowestEdge = LowerEdge;
		if(LeftEdge < LeftestEdge) LeftestEdge = LeftEdge;
		if(RightEdge > RightestEdge) RightestEdge = RightEdge;
	}
	printf("highest edge: %d, lowest edge: %d\n", HighestEdge, LowestEdge);
	printf("leftmost edge: %d, rightmost edge: %d\n", LeftestEdge, RightestEdge);

	int BoxWidth = RightestEdge - LeftestEdge;
	int BoxHeight = LowestEdge - HighestEdge;
	printf("box width: %d, box height: %d\n", BoxWidth, BoxHeight);

	int FinalWidth = BoxWidth * NumGlyphs;
	int FinalHeight = BoxHeight;
	unsigned char *FinalBitmap = (unsigned char *)malloc(FinalWidth * FinalHeight);
	for(int Y = 0; Y < FinalHeight; ++Y)
	{
		for(int X = 0; X < FinalWidth; ++X)
		{
			int Index = Y * FinalWidth + X;
//			FinalBitmap[Index] = 0x88;
			FinalBitmap[Index] = 0x00;
		}
	}

	int Baseline = -HighestEdge;
	int PenX = 0;
	int PenY = Baseline;
	for(int i = 0; i < NumGlyphs; ++i)
	{
		glyph Glyph = Glyphs[i];
		int GlyphX = PenX + Glyph.XOff;
		int GlyphY = PenY + Glyph.YOff;
		for(int SrcY = 0, DestY = GlyphY; SrcY < Glyph.Height; ++SrcY, ++DestY)
		{
			for(int SrcX = 0, DestX = GlyphX; SrcX < Glyph.Width; ++SrcX, ++DestX)
			{
				FinalBitmap[DestY * FinalWidth + DestX] = Glyph.Bitmap[SrcY * Glyph.Width + SrcX];
			}
		}

		PenX += BoxWidth;
	}

	*Width = FinalWidth;
	*Height = FinalHeight;
	return FinalBitmap;
}

int main()
{
	int Width, Height;
//	const char *FontFile = "/home/eero/all/no-gtk-text-editor/ttf2image/novem___.ttf";
	const char *FontFile = "/home/eero/.local/share/fonts/FiraMono-Medium.ttf";
	double FontSize = 8.0;
//	double FontSize = 16.0;
//	double FontSize = 32.0;
//	double FontSize = 64.0;
	unsigned char *Bitmap = make_font_image(FontFile, FontSize, &Width, &Height);
	if(!Bitmap) return 1;

	int NumComponents = 1;
	int StrideInBytes = Width * NumComponents;
	int Result = stbi_write_png("test.png", Width, Height, NumComponents, (const void *)Bitmap, StrideInBytes);
	if(!Result)
	{
		fprintf(stderr, "error: stbi_write_png()\n");
		return 1;
	}

	return 0;
}


