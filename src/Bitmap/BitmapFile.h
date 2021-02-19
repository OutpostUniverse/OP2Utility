#pragma once

#include "Color.h"
#include "BmpHeader.h"
#include "ImageHeader.h"
#include <vector>
#include <string>
#include <cstdint>
#include <cstddef>

namespace OP2Utility
{
	namespace Stream {
		class Writer;
		class BidirectionalReader;
	}

	enum class ScanLineOrientation
	{
		TopDown,
		BottomUp
	};

	// BitmapFile only supports indexed palette BMPs. 
	// Writing and reading bitmaps is restricted to the subset of 1, 2 and 8 bit BMPs.
	class BitmapFile
	{
	public:
		BmpHeader bmpHeader;
		ImageHeader imageHeader;
		std::vector<Color> palette;
		std::vector<uint8_t> pixels;

		static bool PeekIsBitmap(Stream::BidirectionalReader& reader);
		static bool PeekIsBitmap(Stream::BidirectionalReader&& reader);

		static BitmapFile CreateIndexed(uint16_t bitCount, uint32_t width, int32_t height);
		static BitmapFile CreateIndexed(uint16_t bitCount, uint32_t width, int32_t height, std::vector<Color> palette);
		static BitmapFile CreateIndexed(uint16_t bitCount, uint32_t width, int32_t height, std::vector<Color> palette, std::vector<uint8_t> pixels);

		// BMP Reader only supports indexed color palettes (1, 2, and 8 bit BMPs).
		static BitmapFile ReadIndexed(const std::string& filename);
		static BitmapFile ReadIndexed(Stream::BidirectionalReader& reader);
		static BitmapFile ReadIndexed(Stream::BidirectionalReader&& reader);

		// BMP Writer only supports indexed color palettes (1, 2, and 8 bit BMPs).
		// @indexedPixels: Must include padding to fill each image row out to the next 4 byte memory border (pitch).
		void WriteIndexed(std::string filename) const;
		void WriteIndexed(Stream::Writer& writer) const;
		void WriteIndexed(Stream::Writer&& writer) const;

		void VerifyIndexedPaletteSizeDoesNotExceedBitCount() const;
		static void VerifyIndexedPaletteSizeDoesNotExceedBitCount(uint16_t bitCount, std::size_t paletteSize);

		// Check the pixel count is correct and already includes dummy pixels out to next 4 byte boundary.
		// @width: Width in pixels. Do not include the pitch in width.
		// @pixelsWithPitchSize: Number of pixels including padding pixels to next 4 byte boundary.
		void VerifyPixelSizeMatchesImageDimensionsWithPitch() const;
		static void VerifyPixelSizeMatchesImageDimensionsWithPitch(uint16_t bitCount, int32_t width, int32_t height, std::size_t pixelsWithPitchSize);

		void Validate() const;

		ScanLineOrientation GetScanLineOrientation() const;
		uint32_t AbsoluteHeight() const;
		void SwapRedAndBlue();
		void InvertScanLines();

	private:
		static void VerifyIndexedImageForSerialization(uint16_t bitCount);

		// Read
		static BmpHeader ReadBmpHeader(Stream::BidirectionalReader& seekableReader);
		static ImageHeader ReadImageHeader(Stream::BidirectionalReader& seekableReader);
		static void ReadPalette(Stream::BidirectionalReader& seekableReader, BitmapFile& bitmapFile);
		static void ReadPixels(Stream::BidirectionalReader& seekableReader, BitmapFile& bitmapFile);

		// Write
		static void WriteHeaders(Stream::Writer& seekableWriter, uint16_t bitCount, int width, int height, const std::vector<Color>& palette);
		static void WritePixels(Stream::Writer& seekableWriter, const std::vector<uint8_t>& pixels, int32_t width, int32_t height, uint16_t bitCount);
	};

	bool operator==(const BitmapFile& lhs, const BitmapFile& rhs);
	bool operator!=(const BitmapFile& lhs, const BitmapFile& rhs);
}
