#include "IndexedBmpWriter.h"
#include "ImageHeader.h"
#include "../Streams/FileWriter.h"
#include "../Streams/SeekableReader.h"
#include <algorithm>
#include <stdexcept>
#include <cmath>

void IndexedBmpWriter::WriteScanLineIncluded(std::string filename, uint16_t bitCount, int32_t width, int32_t height, const std::vector<Color>& palette, const std::vector<uint8_t>& pixelsWithScanLine)
{
	ImageHeader::CheckIndexedBitCount(bitCount);
	CheckPaletteCount(bitCount, palette.size());
	CheckPixelCountWithScanLine(bitCount, width, height, pixelsWithScanLine.size());
	CheckPixelIndices(bitCount, palette.size(), pixelsWithScanLine);

	Stream::FileWriter fileWriter(filename);

	WriteHeaders(fileWriter, bitCount, width, height, palette);
	
	fileWriter.Write(palette);
	fileWriter.Write(pixelsWithScanLine);
}

// Writes a Bitmap with an indexed color palette
void IndexedBmpWriter::Write(std::string filename, uint16_t bitCount, int32_t width, int32_t height, const std::vector<Color>& palette, const std::vector<uint8_t>& pixels)
{
	ImageHeader::CheckIndexedBitCount(bitCount);
	CheckPaletteCount(bitCount, palette.size());
	CheckPixelCount(bitCount, std::abs(height) * width, pixels.size());

	std::size_t pixelOffset = 14 + sizeof(ImageHeader) + palette.size() * sizeof(Color);
	std::size_t headerSize = pixelOffset + pixels.size() * sizeof(uint8_t);

	if (headerSize > UINT32_MAX) {
		throw std::runtime_error("Bitmap size is too large to save to disk.");
	}

	BmpHeader bmpHeader = BmpHeader::Create(static_cast<uint32_t>(headerSize), static_cast<uint32_t>(pixelOffset));
	ImageHeader imageHeader = ImageHeader::Create(width, height, bitCount);

	Stream::FileWriter fileWriter(filename);

	fileWriter.Write(bmpHeader.type);
	fileWriter.Write(bmpHeader.size);
	fileWriter.Write(bmpHeader.reserved1);
	fileWriter.Write(bmpHeader.reserved2);
	fileWriter.Write(bmpHeader.pixelOffset);

	fileWriter.Write(imageHeader);
	fileWriter.Write(palette);

	// If height > 0, top line of pixels is bottom line in file. 
	// If height < 0, top line of pixels is top line in file
	if (height < 0) {
		WritePixelsTopDown(fileWriter, bitCount, width, height, pixels);
	}
	else {
		WritePixelsBottomUp(fileWriter, bitCount, width, height, pixels);
	}
}

void IndexedBmpWriter::WriteHeaders(Stream::SeekableWriter& seekableWriter, uint16_t bitCount, int width, int height, const std::vector<Color>& palette)
{
	std::size_t pixelOffset = 14 + sizeof(ImageHeader) + palette.size() * sizeof(Color);
	std::size_t fileSize = pixelOffset + CalculateScanLineSize(bitCount, width) * height;

	if (fileSize > UINT32_MAX) {
		throw std::runtime_error("Bitmap size is too large to save to disk.");
	}

	BmpHeader bmpHeader = BmpHeader::Create(static_cast<uint32_t>(fileSize), static_cast<uint32_t>(pixelOffset));
	ImageHeader imageHeader = ImageHeader::Create(width, height, bitCount);

	seekableWriter.Write(bmpHeader.type);
	seekableWriter.Write(bmpHeader.size);
	seekableWriter.Write(bmpHeader.reserved1);
	seekableWriter.Write(bmpHeader.reserved2);
	seekableWriter.Write(bmpHeader.pixelOffset);

	seekableWriter.Write(imageHeader);
}

int32_t IndexedBmpWriter::CalculateScanLineSize(uint16_t bitCount, int32_t width)
{
	const uint16_t bytesOfPixelsPerRow = width / (8 / bitCount);

	return ( (bytesOfPixelsPerRow + 3) & ~3 );
}

void IndexedBmpWriter::WritePixelsTopDown(Stream::SeekableWriter& fileWriter, uint16_t bitCount, int32_t width, int32_t height, const std::vector<uint8_t>& pixels)
{
	std::vector<uint8_t> buffer;
	const uint16_t bytesOfSetPixelsPerRow = width / 8 / bitCount;
	buffer.resize((bytesOfSetPixelsPerRow + 3) & ~3);
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
	std::vector<uint8_t> buffer;
	const uint16_t bytesOfSetPixelsPerRow = width / 8 / bitCount;
	buffer.resize((bytesOfSetPixelsPerRow + 3) & ~3);
	int index = bytesOfSetPixelsPerRow * height; //Index is in bytes, not necessarily pixels

	for (int row = 0; row < height; ++row)
	{
		// 0 pad the end of each line so it is a multiple of 4 bytes
		std::fill(buffer.begin(), buffer.end(), 0);
		std::copy(pixels.begin() + index - bytesOfSetPixelsPerRow,
			pixels.begin() + index,
			buffer.begin());
		fileWriter.Write(buffer);

		index -= bytesOfSetPixelsPerRow;
	}
}

void IndexedBmpWriter::CheckPaletteCount(uint16_t bitCount, std::size_t paletteSize)
{
	if (paletteSize > static_cast<uint16_t>(2 << bitCount)) {
		throw std::runtime_error("Too many colors listed on the indexed palette");
	}
}

// pixelContainerSize: Number of entries in the pixel container. 
//                     Each entry will represent multiple pixels for a 1 or 4 bit count.
void IndexedBmpWriter::CheckPixelCount(uint16_t bitCount, std::size_t pixelCount, std::size_t pixelContainerSize) 
{
	if (pixelCount != pixelContainerSize * 8 / bitCount) {
		throw std::runtime_error("Number of expected pixels does not match size of pixel container");
	}
}

void IndexedBmpWriter::CheckPixelCountWithScanLine(uint16_t bitCount, int32_t width, int32_t height, std::size_t pixelCountIncludingScanLine)
{
	const uint16_t pixelsPerByte = 8 / bitCount;

	if (pixelCountIncludingScanLine * pixelsPerByte != CalculateScanLineSize(bitCount, width) * std::abs(height)) {
		throw std::runtime_error("An incorrect number of pixels were passed.");
	}
}

void IndexedBmpWriter::CheckPixelIndices(uint16_t bitCount, std::size_t paletteCount, const std::vector<uint8_t>& pixels)
{
	// Check if palette is full
	if (paletteCount == 2 << bitCount) {
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
		if (pixels[i] >= paletteCount) {
			throw std::runtime_error("Pixel " + std::to_string(i) + " is outside the range of set palette indices.");
		}
	}
}
