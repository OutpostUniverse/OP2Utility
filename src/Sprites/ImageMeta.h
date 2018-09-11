#pragma once

#include "ImageType.h"
#include <cstdint>

struct ImageMeta {
	uint32_t scanLineByteWidth; //number of bytes in each scan line of image (this should be the width of the image rounded up to a 32 bit boundary)
	uint32_t pixelDataOffset; // Offset of the pixel data in the .bmp file
	uint32_t height; // Height of image in pixels
	uint32_t width; // Width of image in pixels
	ImageType type;
	uint16_t paletteIndex;
};
