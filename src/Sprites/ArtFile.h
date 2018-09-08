#pragma once

#include "../Rect.h"
#include "ImageType.h"
#include "Color.h"
#include <vector>
#include <cstdint>

struct ImageMeta {
	uint32_t scanlineByteWidth; //number of bytes in each scanline of image (this should be the width of the image rounded up to a 32 bit boundary)
	uint32_t pixelDataOffset; // Offset of the pixel data in the .bmp file
	uint32_t height; // Height of image in pixels
	uint32_t width; // Width of image in pixels
	ImageType type;
	uint16_t paletteIndex;
};

struct UnknownArtContainer {
	uint32_t unknown1;
	uint32_t unknown2;
	uint32_t unknown3;
	uint32_t unknown4;
};

// Note: order determined by local pic_index
struct SubFrame {
	uint16_t bitmapIndex;
	uint8_t unknown;
	uint8_t frameIndex;
	uint16_t pixelXOffset;
	uint16_t pixelYOffset;
};

struct Frame {
	uint8_t unknown;
	uint8_t optional1;
	uint8_t optional2;
	uint8_t optional3;
	uint8_t optional4;

	// Limited to size of uint16_t MAX
	std::vector<SubFrame> subFrames;
};

struct Animation {
	uint32_t unknown;
	Rect selectionRect; // pixels
	uint32_t pixelXDisplacement; // Reverse direction from an offset, origin is center of tile
	uint32_t pixelYDisplacement; // reverse direction from an offset, origin is center of tile
	uint32_t unknown2; //(0x3C CC/DIRT/Garage/Std. Lab construction/dock, 0x0D spider walking)

	std::vector<Frame> frames;

	std::vector<UnknownArtContainer> unknownContainer;
};

struct ArtFile
{
public:
	std::vector<Palette> palettes;
	std::vector<ImageMeta> imageMetas;
	std::vector<Animation> animations;

	void ValidateImageMetadata() const;
};
