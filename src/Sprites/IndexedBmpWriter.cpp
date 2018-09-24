#include "IndexedBmpWriter.h"
#include "ImageHeader.h"
#include "../Streams/FileWriter.h"
#include "../Streams/SeekableReader.h"
#include <algorithm>
#include <stdexcept>
#include <cmath>

void IndexedBmpWriter::WritePixelsIncludingPadding(std::string filename, uint16_t bitCount, int32_t width, int32_t height, const std::vector<Color>& palette, const std::vector<uint8_t>& pixelsWithPadding)
{
	ImageHeader::VerifyIndexedBitCount(bitCount);
	VerifyPaletteSizeDoesNotExceedBitCount(bitCount, palette.size());
	VerifyPixelBufferSizeMatchesImageDimensionsWithPitch(bitCount, width, height, pixelsWithPadding.size());
	VerifyPixelsContainedInPalette(bitCount, palette.size(), pixelsWithPadding);

	Stream::FileWriter fileWriter(filename);

	WriteHeaders(fileWriter, bitCount, width, height, palette);
	fileWriter.Write(palette);
	fileWriter.Write(pixelsWithPadding);
}

// Writes a Bitmap with an indexed color palette
void IndexedBmpWriter::Write(std::string filename, uint16_t bitCount, int32_t width, int32_t height, const std::vector<Color>& palette, const std::vector<uint8_t>& pixels)
{
	ImageHeader::VerifyIndexedBitCount(bitCount);
	VerifyPaletteSizeDoesNotExceedBitCount(bitCount, palette.size());
	VerifyPixelBufferSizeMatchesImageDimensions(bitCount, std::abs(height) * width, pixels.size());
	VerifyPixelsContainedInPalette(bitCount, palette.size(), pixels);

	Stream::FileWriter fileWriter(filename);

	WriteHeaders(fileWriter, bitCount, width, height, palette);
	fileWriter.Write(palette);
	WritePixels(fileWriter, bitCount, width, height, pixels);
}

void IndexedBmpWriter::WriteHeaders(Stream::SeekableWriter& seekableWriter, uint16_t bitCount, int width, int height, const std::vector<Color>& palette)
{
	std::size_t pixelOffset = sizeof(BmpHeader) + sizeof(ImageHeader) + palette.size() * sizeof(Color);
	std::size_t fileSize = pixelOffset + CalcScanlinePitch(bitCount, width) * std::abs(height);

	if (fileSize > UINT32_MAX) {
		throw std::runtime_error("Bitmap size is too large to save to disk.");
	}

	BmpHeader bmpHeader = BmpHeader::Create(static_cast<uint32_t>(fileSize), static_cast<uint32_t>(pixelOffset));
	ImageHeader imageHeader = ImageHeader::Create(width, height, bitCount);

	seekableWriter.Write(bmpHeader);
	seekableWriter.Write(imageHeader);
}

void IndexedBmpWriter::WritePixels(Stream::SeekableWriter& seekableWriter, uint16_t bitCount, int32_t width, int32_t height, const std::vector<uint8_t>& pixels)
{
	// If height > 0, top line of pixels is bottom line in file. 
	// If height < 0, top line of pixels is top line in file
	if (height < 0) {
		WritePixelsTopDown(seekableWriter, bitCount, width, height, pixels);
	}
	else {
		WritePixelsBottomUp(seekableWriter, bitCount, width, height, pixels);
	}
}

int32_t IndexedBmpWriter::CalcScanlinePitch(uint16_t bitCount, int32_t width)
{
	const uint16_t bytesOfPixelsPerRow = CalcScanlineByteWidth(bitCount, width);

	return ( (bytesOfPixelsPerRow + 3) & ~3 );
}

void IndexedBmpWriter::WritePixelsTopDown(Stream::SeekableWriter& fileWriter, uint16_t bitCount, int32_t width, int32_t height, const std::vector<uint8_t>& pixels)
{
	const uint16_t bytesOfSetPixelsPerRow = CalcScanlineByteWidth(bitCount, width);
	std::vector<uint8_t> buffer( (bytesOfSetPixelsPerRow + 3) & ~3 );
	int index = 0; //Index is in bytes, not necessarily pixels

	for (int row = 0; row < -1 * height; ++row)
	{
		// 0 pad the end of each line so it is a multiple of 4 bytes
		std::fill(buffer.begin(), buffer.end(), 0);
		std::copy(pixels.begin() + index,
			pixels.begin() + index + bytesOfSetPixelsPerRow,
			buffer.begin());
		fileWriter.Write(buffer);

		index += bytesOfSetPixelsPerRow;
	}
}

void IndexedBmpWriter::WritePixelsBottomUp(Stream::SeekableWriter& fileWriter, uint16_t bitCount, int32_t width, int32_t height, const std::vector<uint8_t>& pixels)
{
	const uint16_t bytesOfPixelsPerRow = CalcScanlineByteWidth(bitCount, width);
	std::vector<uint8_t> buffer( (bytesOfPixelsPerRow + 3) & ~3 );
	int index = bytesOfPixelsPerRow * height; //Index is in bytes, not necessarily pixels

	for (int row = 0; row < height; ++row)
	{
		// 0 pad the end of each line so it is a multiple of 4 bytes
		std::fill(buffer.begin(), buffer.end(), 0);
		std::copy(pixels.begin() + index - bytesOfPixelsPerRow,
			pixels.begin() + index,
			buffer.begin());
		fileWriter.Write(buffer);

		index -= bytesOfPixelsPerRow;
	}
}

uint32_t IndexedBmpWriter::CalcScanlineByteWidth(uint16_t bitCount, int32_t width)
{
	const uint16_t bitsPerByte = 8;
	return width * bitCount / bitsPerByte;
}

void IndexedBmpWriter::VerifyPaletteSizeDoesNotExceedBitCount(uint16_t bitCount, std::size_t paletteSize)
{
	if (paletteSize > static_cast<uint16_t>(2 << bitCount)) {
		throw std::runtime_error("Too many colors listed on the indexed palette");
	}
}

// pixelContainerSize: Number of entries in the pixel container. 
//                     Each entry will represent multiple pixels for a 1 or 4 bit count.
void IndexedBmpWriter::VerifyPixelBufferSizeMatchesImageDimensions(uint16_t bitCount, std::size_t pixelCount, std::size_t pixelContainerSize) 
{
	if (pixelCount != pixelContainerSize * (8 / bitCount)) {
		throw std::runtime_error("Number of expected pixels does not match size of pixel container");
	}
}

void IndexedBmpWriter::VerifyPixelBufferSizeMatchesImageDimensionsWithPitch(uint16_t bitCount, int32_t width, int32_t height, std::size_t pixelCountIncludingPitch)
{
	const uint16_t pixelsPerByte = 8 / bitCount;

	// G++ will flag warning -Wsign-compare if comparing a signed and unsigned value
	if (pixelCountIncludingPitch * pixelsPerByte != static_cast<uint32_t>(CalcScanlinePitch(bitCount, width) * std::abs(height))) {
		throw std::runtime_error("An incorrect number of pixels were passed.");
	}
}

void IndexedBmpWriter::VerifyPixelsContainedInPalette(uint16_t bitCount, std::size_t paletteEntryCount, const std::vector<uint8_t>& pixels)
{
	// Check if palette is full
	// G++ will flag warning -Wsign-compare if comparing a signed and unsigned value
	// MSVC will flag warning C4334 on x64 compilation if bit shift not set to 64 bit integer
	if (paletteEntryCount == static_cast<uint64_t>(1) << bitCount) {
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
