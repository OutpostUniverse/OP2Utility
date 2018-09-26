#include "IndexedBmpWriter.h"
#include "ImageHeader.h"
#include "../Streams/FileWriter.h"
#include "../Streams/SeekableReader.h"
#include <stdexcept>
#include <cmath>

void IndexedBmpWriter::Write(std::string filename, uint16_t bitCount, int32_t width, int32_t height, const std::vector<Color>& palette, const std::vector<uint8_t>& indexedPixels)
{
	ImageHeader::VerifyIndexedBitCount(bitCount);
	VerifyPaletteSizeDoesNotExceedBitCount(bitCount, palette.size());
	VerifyPixelBufferSizeMatchesImageDimensionsWithPitch(bitCount, width, height, indexedPixels.size());
	VerifyPixelsContainedInPalette(bitCount, palette.size(), indexedPixels);

	Stream::FileWriter fileWriter(filename);

	WriteHeaders(fileWriter, bitCount, width, height, palette);
	fileWriter.Write(palette);
	fileWriter.Write(indexedPixels);
}

void IndexedBmpWriter::WriteHeaders(Stream::SeekableWriter& seekableWriter, uint16_t bitCount, int width, int height, const std::vector<Color>& palette)
{
	std::size_t pixelOffset = sizeof(BmpHeader) + sizeof(ImageHeader) + palette.size() * sizeof(Color);
	std::size_t fileSize = pixelOffset + CalculatePitch(bitCount, width) * std::abs(height);

	if (fileSize > UINT32_MAX) {
		throw std::runtime_error("Bitmap size is too large to save to disk.");
	}

	BmpHeader bmpHeader = BmpHeader::Create(static_cast<uint32_t>(fileSize), static_cast<uint32_t>(pixelOffset));
	ImageHeader imageHeader = ImageHeader::Create(width, height, bitCount);

	seekableWriter.Write(bmpHeader);
	seekableWriter.Write(imageHeader);
}

unsigned int IndexedBmpWriter::CalculatePitch(uint16_t bitCount, int32_t width)
{
	const uint16_t bytesOfPixelsPerRow = CalcPixelByteWidth(bitCount, width);

	return ( (bytesOfPixelsPerRow + 3) & ~3 );
}

unsigned int IndexedBmpWriter::CalcPixelByteWidth(uint16_t bitCount, int32_t width)
{
	const uint16_t bitsPerByte = 8;
	return ((width * bitCount) + (bitsPerByte - 1)) / bitsPerByte;
}

void IndexedBmpWriter::VerifyPaletteSizeDoesNotExceedBitCount(uint16_t bitCount, std::size_t paletteSize)
{
	if (paletteSize > static_cast<uint16_t>(2 << bitCount)) {
		throw std::runtime_error("Too many colors listed on the indexed palette");
	}
}

void IndexedBmpWriter::VerifyPixelBufferSizeMatchesImageDimensionsWithPitch(uint16_t bitCount, int32_t width, int32_t height, std::size_t pixelsWithPitchSize)
{
	if (pixelsWithPitchSize != CalculatePitch(bitCount, width) * std::abs(height)) {
		throw std::runtime_error("An incorrect number of pixels were passed.");
	}
}

void IndexedBmpWriter::VerifyPixelsContainedInPalette(uint16_t bitCount, std::size_t paletteEntryCount, const std::vector<uint8_t>& pixels)
{
	// Check if palette is full
	// Use explicit size_t type to avoid compiler warnings for signedness or size
	if (paletteEntryCount == std::size_t{1} << bitCount) {
		return;
	}

	if (bitCount == 1) {
		for (const auto& pixelGroup : pixels) {
			if (pixelGroup > 0) {
				throw (std::runtime_error("A pixel is outside the range of set palette indices."));
			}
		}
	}

	if (bitCount == 4) {
		return; //TODO: Support checking 2 bit images
	}

	for (std::size_t i = 0; i < pixels.size(); ++i) {
		if (pixels[i] >= paletteEntryCount) {
			throw std::runtime_error("Pixel " + std::to_string(i) + " is outside the range of set palette indices.");
		}
	}
}
