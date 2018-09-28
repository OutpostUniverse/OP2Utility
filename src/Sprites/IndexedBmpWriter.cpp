#include "IndexedBmpWriter.h"
#include "ImageHeader.h"
#include "../Streams/FileWriter.h"
#include "../Streams/SeekableReader.h"
#include <stdexcept>
#include <cmath>

void IndexedBmpWriter::Write(std::string filename, uint16_t bitCount, int32_t width, int32_t height, std::vector<Color> palette, const std::vector<uint8_t>& indexedPixels)
{
	ImageHeader::VerifyIndexedBitCount(bitCount);
	VerifyPaletteSizeDoesNotExceedBitCount(bitCount, palette.size());
	VerifyPixelBufferSizeMatchesImageDimensionsWithPitch(bitCount, width, height, indexedPixels.size());

	palette.resize(std::size_t{ 1 } << bitCount, DiscreteColor::Black);

	Stream::FileWriter fileWriter(filename);

	WriteHeaders(fileWriter, bitCount, width, height, palette);
	fileWriter.Write(palette);

	WritePixels(fileWriter, indexedPixels, width, bitCount);
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

void IndexedBmpWriter::WritePixels(Stream::SeekableWriter& seekableWriter, const std::vector<uint8_t>& pixels, int32_t width, uint16_t bitCount)
{
	const unsigned int pitch = CalculatePitch(bitCount, width);
	const unsigned int bytesOfPixelsPerRow = CalcPixelByteWidth(bitCount, width);
	const std::vector<uint8_t> padding(pitch - bytesOfPixelsPerRow, 0);

	for (std::size_t i = 0; i < pixels.size();) {
		seekableWriter.Write(&pixels[i], bytesOfPixelsPerRow);
		seekableWriter.Write(padding);
		i += pitch;
	}
}

unsigned int IndexedBmpWriter::CalculatePitch(uint16_t bitCount, int32_t width)
{
	const unsigned int bytesOfPixelsPerRow = CalcPixelByteWidth(bitCount, width);
	return ( (bytesOfPixelsPerRow + 3) & ~3 );
}

unsigned int IndexedBmpWriter::CalcPixelByteWidth(uint16_t bitCount, int32_t width)
{
	const unsigned int bitsPerByte = 8;
	return ((width * bitCount) + (bitsPerByte - 1)) / bitsPerByte;
}

void IndexedBmpWriter::VerifyPaletteSizeDoesNotExceedBitCount(uint16_t bitCount, std::size_t paletteSize)
{
	if (paletteSize > std::size_t{ 1 } << bitCount) {
		throw std::runtime_error("Too many colors listed on the indexed palette");
	}
}

void IndexedBmpWriter::VerifyPixelBufferSizeMatchesImageDimensionsWithPitch(uint16_t bitCount, int32_t width, int32_t height, std::size_t pixelsWithPitchSize)
{
	if (pixelsWithPitchSize != CalculatePitch(bitCount, width) * std::abs(height)) {
		throw std::runtime_error("An incorrect number of pixels were passed.");
	}
}
