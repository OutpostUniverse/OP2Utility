#pragma once

#include "Color.h"
#include "BmpHeader.h"
#include "ImageHeader.h"
#include <vector>
#include <string>
#include <cstdint>
#include <cstddef>

namespace Stream {
	class BidirectionalSeekableWriter;
	class BidirectionalSeekableReader;
}

// BMP Writer only supporting Indexed Color palettes (1, 2, and 8 bit BMPs). 
class BitmapFile
{
public:
	BmpHeader bmpHeader;
	ImageHeader imageHeader;
	std::vector<Color> palette;
	std::vector<uint8_t> pixels; // Includes pitch

	// Uses a default 4 byte pitch
	static BitmapFile CreateDefaultIndexed(uint16_t bitCount, uint32_t width, uint32_t height);

	// BMP Reader only supports indexed color palettes (1, 2, and 8 bit BMPs).
	static BitmapFile ReadIndexed(const std::string& filename);
	static BitmapFile ReadIndexed(Stream::BidirectionalSeekableReader& seekableReader);

	// BMP Writer only supports indexed color palettes (1, 2, and 8 bit BMPs).
	// @indexedPixels: Must include padding to fill each image row out to the next byte memory border (pitch). 4 byte pitch is typical.
	static void WriteIndexed(std::string filename, uint16_t bitCount, int32_t width, int32_t height, std::size_t pitch, std::vector<Color> palette, const std::vector<uint8_t>& indexedPixels);
	static void WriteIndexed(Stream::BidirectionalSeekableWriter& seekableWriter, uint16_t bitCount, int32_t width, int32_t height, std::size_t pitch, std::vector<Color> palette, const std::vector<uint8_t>& indexedPixels);
	static void WriteIndexed(std::string filename, const BitmapFile& bitmapFile);

	void VerifyIndexedPaletteSizeDoesNotExceedBitCount() const;
	static void VerifyIndexedPaletteSizeDoesNotExceedBitCount(uint16_t bitCount, std::size_t paletteSize);

	// Check the pixel count is correct and already includes dummy pixels out to next 4 byte boundary.
	// @width: Width in pixels. Do not include the pitch in width.
	// @pixelsWithPitchSize: Number of pixels including padding pixels to next 4 byte boundary.
	void VerifyPixelSizeMatchesImageDimensionsWithPitch() const;
	static void VerifyPixelSizeMatchesImageDimensionsWithPitch(uint16_t bitCount, std::size_t pitch, int32_t height, std::size_t pixelsWithPitchSize);
	
	std::size_t FindPitch() const;
	static std::size_t FindPitch(std::size_t width, std::size_t height, std::size_t pixelCount);

	void Validate() const;

private:
	static void VerifyIndexedImageForSerialization(uint16_t bitCount);

	// Read
	static BmpHeader ReadBmpHeader(Stream::BidirectionalSeekableReader& seekableReader);
	static ImageHeader ReadImageHeader(Stream::BidirectionalSeekableReader& seekableReader);
	static void ReadPalette(Stream::BidirectionalSeekableReader& seekableReader, BitmapFile& bitmapFile);
	static void ReadPixels(Stream::BidirectionalSeekableReader& seekableReader, BitmapFile& bitmapFile);

	// Write
	static void WriteHeaders(Stream::BidirectionalSeekableWriter& seekableWriter, uint16_t bitCount, int width, int height, std::size_t pitch, const std::vector<Color>& palette);
	static void WritePixels(Stream::BidirectionalSeekableWriter& seekableWriter, const std::vector<uint8_t>& pixels, int32_t width, uint16_t bitCount, std::size_t pitch);
};

bool operator==(const BitmapFile& lhs, const BitmapFile& rhs);
bool operator!=(const BitmapFile& lhs, const BitmapFile& rhs);
