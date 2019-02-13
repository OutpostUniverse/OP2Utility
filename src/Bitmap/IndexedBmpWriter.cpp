#include "BitmapFile.h"
#include "../Stream/FileWriter.h"
#include <stdexcept>
#include <cmath>

void BitmapFile::WriteIndexed(std::string filename, const BitmapFile& bitmapFile)
{
	// Test all properties that are auto-generated as correct when writing bitmap piecemeal
	if (bitmapFile.imageHeader.compression != BmpCompression::Uncompressed) {
		throw std::runtime_error("Unable to write compressed bitmap files");
	}

	bitmapFile.Validate();

	WriteIndexed(filename, bitmapFile.imageHeader.bitCount, bitmapFile.imageHeader.width, bitmapFile.imageHeader.height, bitmapFile.palette, bitmapFile.pixels);
}

void BitmapFile::WriteIndexed(std::string filename, uint16_t bitCount, int32_t width, int32_t height, std::vector<Color> palette, const std::vector<uint8_t>& indexedPixels)
{
	Stream::FileWriter fileWriter(filename);
	WriteIndexed(fileWriter, bitCount, width, height, palette, indexedPixels);
}

void BitmapFile::WriteIndexed(Stream::BidirectionalWriter& seekableWriter, uint16_t bitCount, int32_t width, int32_t height, std::vector<Color> palette, const std::vector<uint8_t>& indexedPixels)
{
	VerifyIndexedImageForSerialization(bitCount);
	VerifyIndexedPaletteSizeDoesNotExceedBitCount(bitCount, palette.size());
	VerifyPixelSizeMatchesImageDimensionsWithPitch(bitCount, width, height, indexedPixels.size());

	palette.resize(ImageHeader::CalcMaxIndexedPaletteSize(bitCount), DiscreteColor::Black);

	WriteHeaders(seekableWriter, bitCount, width, height, palette);
	seekableWriter.Write(palette);

	WritePixels(seekableWriter, indexedPixels, width, bitCount);
}

void BitmapFile::WriteHeaders(Stream::BidirectionalWriter& seekableWriter, uint16_t bitCount, int width, int height, const std::vector<Color>& palette)
{
	std::size_t pixelOffset = sizeof(BmpHeader) + sizeof(ImageHeader) + palette.size() * sizeof(Color);
	std::size_t fileSize = pixelOffset + ImageHeader::CalculatePitch(bitCount, width) * std::abs(height);

	if (fileSize > UINT32_MAX) {
		throw std::runtime_error("Bitmap size is too large to save to disk.");
	}

	auto bmpHeader = BmpHeader::Create(static_cast<uint32_t>(fileSize), static_cast<uint32_t>(pixelOffset));
	auto imageHeader = ImageHeader::Create(width, height, bitCount);

	seekableWriter.Write(bmpHeader);
	seekableWriter.Write(imageHeader);
}

void BitmapFile::WritePixels(Stream::BidirectionalWriter& seekableWriter, const std::vector<uint8_t>& pixels, int32_t width, uint16_t bitCount)
{
	const auto pitch = ImageHeader::CalculatePitch(bitCount, width);
	const auto bytesOfPixelsPerRow = ImageHeader::CalcPixelByteWidth(bitCount, width);
	const std::vector<uint8_t> padding(pitch - bytesOfPixelsPerRow, 0);

	for (std::size_t i = 0; i < pixels.size();) {
		seekableWriter.Write(&pixels[i], bytesOfPixelsPerRow);
		seekableWriter.Write(padding);
		i += pitch;
	}
}
