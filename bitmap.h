#ifndef _BITMAP_H_
#define _BITMAP_H_

#include <stdint.h>
#include <stddef.h>

#pragma pack(push, 1)
typedef struct bitmap_header_t
{
	int8_t signature[2];			// BM
	int32_t file_size;			// sizeof(struct bitmap_header_t) + data
	int32_t reserved;			// 0
	int32_t offset;				// offsetof(offset) + 40

	int32_t dib_size;			// 40
	int32_t width;
	int32_t height;
	int16_t planes;				// 1
	int16_t bit_per_pixel;			// 32
	int32_t compression;			// 0
	int32_t size;				// data
	int32_t horizontal_resolution;		// 2835
	int32_t vertical_resolution;		// 2835
	int32_t colors_in_palette;		// 0
	int32_t important_colors;		// 0
} bitmap_header_t;
#pragma pack(pop)

#define BITMAP_HEADER_DEFAULT { "BM", sizeof(bitmap_header_t), 0, offsetof(bitmap_header_t, offset) + 40,\
	40, 0, 0, 1, 32, 0, 0, 2835, 2835, 0, 0}

void bitmap_write(bitmap_header_t *h, uint32_t *buffer, int width, int height, const char *fname);

#endif
