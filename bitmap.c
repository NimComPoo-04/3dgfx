#include <stdlib.h>
#include <stdio.h>

#include "bitmap.h"

void bitmap_write(bitmap_header_t *h, uint32_t *buffer, int width, int height, const char *fname)
{
	bitmap_header_t header = h ? *h : (bitmap_header_t)BITMAP_HEADER_DEFAULT;

	header.size = width * height * sizeof(uint32_t);
	header.file_size += header.size;
	header.width = width;
	header.height = height;

	FILE *f = fopen(fname, "wb+");

	fwrite(&header, sizeof header, 1, f);
	fwrite(buffer, header.size, 1, f);

	fclose(f);
}
