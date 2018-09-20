#pragma once

#include "Color.h"
#include "BmpHeader.h"
#include <array>
#include <vector>
#include <string>
#include <cstdint>
#include <cstddef>

enum class BitCount : uint16_t;

namespace Stream {
	class SeekableReader;
	class SeekableWriter;
}

// BMP Writer only supporting Indexed Color palettes (1, 2, and 8 bit BMPs). 
// Properly writes BMPs with pixels in Bottom Up format (positive height) and Top Down format (negative height)
class IndexedBmpWriter
{
public:
	// The pixel container includes dummy information to fill each image row out to the next 4 byte memory border
	static void WriteScanLineIncluded(std::string filename, uint16_t bitCount, int32_t width, int32_t height, const std::vector<Color>& palette, const std::vector<uint8_t>&);

	// Dummy information is inserted at the end of each pixel row to reach the next 4 byte memory border
	static void Write(std::string filename, uint16_t bitCount, int32_t width, int32_t height, const std::vector<Color>& palette, const std::vector<uint8_t>& indexedPixels);

private:
	static void WriteHeaders(Stream::SeekableWriter& seekableWriter, uint16_t bitCount, int width, int height, const std::vector<Color>& palette);

	static void WritePixels(Stream::SeekableWriter& seekableWriter, uint16_t bitCount, int32_t width, int32_t height, const std::vector<uint8_t>& pixels);

	// If height is a negative number, pixel rows are written top down (as displayed on screen)
	static void WritePixelsTopDown(Stream::SeekableWriter& fileWriter, uint16_t bitCount, int32_t width, int32_t height, const std::vector<uint8_t>& pixels);

	// If height is a positive number, the top row of pixels is written at the end of the file 
	static void WritePixelsBottomUp(Stream::SeekableWriter& fileWriter, uint16_t bitCount, int32_t width, int32_t height, const std::vector<uint8_t>& pixels);

	// Returns pixel row including scan line extension size in bytes
	static int32_t FindScanLineSize(uint16_t bitCount, int32_t width);

	static uint32_t Calc1BytePitch(uint16_t bitCount, int32_t width);

	static void VerifyPaletteCount(uint16_t bitCount, std::size_t paletteSize);
	static void VerifyPixelCount(uint16_t bitCount, std::size_t pixelCount, std::size_t pixelArraySize);

	// Check the pixel count is correct if it already includes dummy pixels out to next 4 byte boundary.
	// @width: Width in pixels. Do not include the scan line in width.
	// @pixelCountIncludingScanLine: Number of pixels including padding pixels to next 4 byte boundary.
	static void VerifyPixelCountWithScanLine(uint16_t bitCount, int32_t width, int32_t height, std::size_t pixelCountIncludingScanLine);

	static void VerifyPixelIndices(uint16_t bitCount, std::size_t paletteSize, const std::vector<uint8_t>& pixels);
};
