#pragma once

#include "Color.h"
#include "BmpHeader.h"
#include <array>
#include <vector>
#include <string>
#include <cstdint>
#include <cstddef>

namespace Stream {
	class SeekableWriter;
}

// BMP Writer only supporting Indexed Color palettes (1, 2, and 8 bit BMPs). 
class IndexedBmpWriter
{
public:
	// @indexedPixels: Must include padding to fill each image row out to the next 4 byte memory border (pitch).
	static void Write(std::string filename, uint16_t bitCount, int32_t width, int32_t height, std::vector<Color> palette, const std::vector<uint8_t>& indexedPixels);


private:
	static void VerifyIndexedImage(uint16_t bitCount);

	static void WriteHeaders(Stream::SeekableWriter& seekableWriter, uint16_t bitCount, int width, int height, const std::vector<Color>& palette);

	static void WritePixels(Stream::SeekableWriter& seekableWriter, const std::vector<uint8_t>& pixels, int32_t width, uint16_t bitCount);

	static std::size_t CalculatePitch(uint16_t bitCount, int32_t width);

	// Does not include padding
	static std::size_t CalcPixelByteWidth(uint16_t bitCount, int32_t width);

	static void VerifyPaletteSizeDoesNotExceedBitCount(uint16_t bitCount, std::size_t paletteSize);

	// Check the pixel count is correct if it already includes dummy pixels out to next 4 byte boundary.
	// @width: Width in pixels. Do not include the pitch in width.
	// @pixelsWithPitchSize: Number of pixels including padding pixels to next 4 byte boundary.
	static void VerifyPixelBufferSizeMatchesImageDimensionsWithPitch(uint16_t bitCount, int32_t width, int32_t height, std::size_t pixelsWithPitchSize);
};
