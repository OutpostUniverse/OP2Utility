#include "BitmapFile.h"
#include "ImageHeader.h"
#include "../Streams/FileWriter.h"
#include "../Streams/SeekableReader.h"
#include <stdexcept>
#include <cmath>

void BitmapFile::WriteIndexed(std::string filename, uint16_t bitCount, int32_t width, int32_t height, std::vector<Color> palette, const std::vector<uint8_t>& indexedPixels)
{
	VerifyIndexedImage(bitCount);
	VerifyPaletteSizeDoesNotExceedBitCount(bitCount, palette.size());
	VerifyPixelSizeMatchesImageDimensionsWithPitch(bitCount, width, height, indexedPixels.size());

	palette.resize(std::size_t{ 1 } << bitCount, DiscreteColor::Black);

	Stream::FileWriter fileWriter(filename);

	WriteHeaders(fileWriter, bitCount, width, height, palette);
	fileWriter.Write(palette);

	WritePixels(fileWriter, indexedPixels, width, bitCount);
}

void BitmapFile::WriteHeaders(Stream::SeekableWriter& seekableWriter, uint16_t bitCount, int width, int height, const std::vector<Color>& palette)
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

void BitmapFile::VerifyIndexedImage(uint16_t bitCount)
{
	if (!ImageHeader::IsIndexedImage(bitCount)) {
		throw std::runtime_error("Unable to write an non-indexed bitmap file. Bit count is " + std::to_string(bitCount) + " but must be 8 or less");
	}
}

void BitmapFile::WritePixels(Stream::SeekableWriter& seekableWriter, const std::vector<uint8_t>& pixels, int32_t width, uint16_t bitCount)
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

void BitmapFile::VerifyPaletteSizeDoesNotExceedBitCount(uint16_t bitCount, std::size_t paletteSize)
{
	if (paletteSize > std::size_t{ 1 } << bitCount) {
		throw std::runtime_error("Too many colors listed on the indexed palette");
	}
}
