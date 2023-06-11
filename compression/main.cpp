#include <stdio.h>
#include <limits.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Lets try to compress the image into 1 bit per pixel bitmap.
//	Using 1 bit per pixel for black and white pixels is a very obvious and effective way to compress such data.
// I dont see very obvious ways to further compress it though. (?)
// We could eliminate repetitions of values by having special bytes that store counts:
//
// * [VALUE] [COUNT] ...
//	That would work and it would certainly eliminate repetitions, but if there are no repetitions then data ends up twice as large.
//
// * [VALUE] [POSSIBLY COUNT] ...
//	Use special bit to recognize a count-byte, but then we cant have values with that bit set anymore. How do we solve that problem?
//
// * Huffman coding

unsigned char *compress(unsigned char *Uncompressed, int NumBytes, int *NumBytesCompressed)
{
	assert(NumBytes % 8 == 0);
	int CompressedSize = NumBytes / 8;
	unsigned char *Compressed = (unsigned char *)malloc(CompressedSize);
	for(int Dest = 0, Src = 0; Dest < CompressedSize; ++Dest, Src += 8)
	{
		Compressed[Dest] = 0;
		Compressed[Dest] |= (Uncompressed[Src+0] ? 1 : 0) << 0;
		Compressed[Dest] |= (Uncompressed[Src+1] ? 1 : 0) << 1;
		Compressed[Dest] |= (Uncompressed[Src+2] ? 1 : 0) << 2;
		Compressed[Dest] |= (Uncompressed[Src+3] ? 1 : 0) << 3;
		Compressed[Dest] |= (Uncompressed[Src+4] ? 1 : 0) << 4;
		Compressed[Dest] |= (Uncompressed[Src+5] ? 1 : 0) << 5;
		Compressed[Dest] |= (Uncompressed[Src+6] ? 1 : 0) << 6;
		Compressed[Dest] |= (Uncompressed[Src+7] ? 1 : 0) << 7;
	}

	*NumBytesCompressed = CompressedSize;
	return Compressed;
}

unsigned char *uncompress(unsigned char *Compressed, int NumBytes)
{
	unsigned char *Uncompressed = 0;

//	int FinalUncompressedSize = FinalCompressedSize * 8;
//	unsigned char *FinalUncompressed = (unsigned char *)malloc(FinalUncompressedSize);
//	for(int i = 0; i < FinalUncompressedSize; ++i) FinalUncompressed[i] = 0x00;
//	for(int Dest = 0, Src = 0; Dest < FinalUncompressedSize; Dest += 8, ++Src)
//	{
//		FinalUncompressed[Dest+0] = (FinalCompressed[Src] & (1 << 0)) ? 0xff : 0x00;
//		FinalUncompressed[Dest+1] = (FinalCompressed[Src] & (1 << 1)) ? 0xff : 0x00;
//		FinalUncompressed[Dest+2] = (FinalCompressed[Src] & (1 << 2)) ? 0xff : 0x00;
//		FinalUncompressed[Dest+3] = (FinalCompressed[Src] & (1 << 3)) ? 0xff : 0x00;
//		FinalUncompressed[Dest+4] = (FinalCompressed[Src] & (1 << 4)) ? 0xff : 0x00;
//		FinalUncompressed[Dest+5] = (FinalCompressed[Src] & (1 << 5)) ? 0xff : 0x00;
//		FinalUncompressed[Dest+6] = (FinalCompressed[Src] & (1 << 6)) ? 0xff : 0x00;
//		FinalUncompressed[Dest+7] = (FinalCompressed[Src] & (1 << 7)) ? 0xff : 0x00;
//	}

	return Uncompressed;
}

void print_bytes(unsigned char *Data, int NumBytes)
{
	for(int i = 0; i < NumBytes; i += 8)
	{
		for(int j = 0; j < 8; ++j)
		{
			printf("0x%.2X,", Data[i+j]);
		}
		printf("\n");
	}
	printf("\n");
}

//void sort_counts(int *Counts, int CountsSize){
//	while(true){
//		bool no_swaps = true;
//		for(int a = 0, b = 1; b < CountsSize; a += 1, b += 1){
//			Identifier *count_a = (Identifier *) counts->data[a];
//			Identifier *count_b = (Identifier *) counts->data[b];
//			if(count_a->count < count_b->count){ // sort in ascending order
//				counts->data[a] = count_b;
//				counts->data[b] = count_a;
//				no_swaps = false;
//			}
//		}
//		if(no_swaps) break; // if no swaps were made, array is sorted
//	}
//}

int main()
{
	const char *File = "test.png";
	int Width, Height, NumChannels;
	stbi_set_flip_vertically_on_load(1);
	unsigned char *Data = stbi_load(File, &Width, &Height, &NumChannels, 0);
	if(!Data)
	{
		fprintf(stderr, "error: stbi_load()\n");
		return 1;
	}
	printf("%s: width: %d, height: %d, num channels: %d\n",
		File, Width, Height, NumChannels);

	int CompressedSize;
	unsigned char *Compressed = compress(Data, Width * Height, &CompressedSize);

//	printf("CompressedSize: %d\n", CompressedSize);
//	print_bytes(Compressed, CompressedSize);

	int Counts[256];
	for(int i = 0; i < 256; ++i) Counts[i] = 0;
	for(int i = 0; i < CompressedSize; ++i)
	{
		unsigned char ByteValue = Compressed[i];
		++Counts[ByteValue];
	}

	unsigned char ByteValues[256]; // stores indices into Counts array
	for(int i = 0; i < 256; ++i)
	{
		ByteValues[i] = (unsigned char)i;
//		ByteValues[i] = (unsigned char)(255 - i);
	}

	// Sort byte values by their count
	for(int i = 0; i < 256; ++i)
	{
		// find largest/smallest value
		int LargestSeen = INT_MIN;
		int k = -1;
		for(int j = i; j < 256; ++j)
		{
			if(Counts[ByteValues[j]] > LargestSeen)
			{
				LargestSeen = Counts[ByteValues[j]];	
				k = j;
			}
		}
		if(i != k)
		{
			// swap values
			unsigned char Temp = ByteValues[i];
			ByteValues[i] = ByteValues[k];
			ByteValues[k] = Temp;
		}
	}

	for(int i = 0; i < 256; ++i)
	{
		unsigned char Value = ByteValues[i];
		int Count = Counts[Value];
		printf("0x%.2X: %d\n", Value, Count);
	}

	return 0;
}





