#pragma once

#include <cstdint>

#pragma pack(push, 1) // Make sure structures are byte aligned

struct ImageMeta {
	uint16_t GetBitCount() const {
		return type.bShadow ? 1 : 8;
	}

	struct ImageType {
		uint16_t bGameGraphic : 1;  // 0 = MenuGraphic, 1 = GameGraphic
		uint16_t unknown1 : 1; // 2
		uint16_t bShadow : 1; // 4
		uint16_t unknown2 : 1; // 8
		uint16_t unknown3 : 1; // 16
		uint16_t unknown4 : 1; // 32
		uint16_t bTruckBed : 1; // 64
		uint16_t unknown5 : 1; // 128
		uint16_t unknown6 : 8;
	};

	static_assert(2 == sizeof(ImageType), "ImageMeta::ImageType is an unexpected size");

	uint32_t scanLineByteWidth; //number of bytes in each scan line of image (this should be the width of the image rounded up to a 32 bit boundary)
	uint32_t pixelDataOffset; // Offset of the pixel data in the .bmp file
	uint32_t height; // Height of image in pixels
	uint32_t width; // Width of image in pixels
	ImageType type;
	uint16_t paletteIndex;
};

static_assert(18 + sizeof(ImageMeta::ImageType) == sizeof(ImageMeta), "ImageMeta is an unexpected size");

#pragma pack(pop)
