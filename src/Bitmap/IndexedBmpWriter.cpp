#include "BitmapFile.h"
#include "../Stream/FileWriter.h"
#include <stdexcept>
#include <cmath>
#include <cstdlib>


void BitmapFile::WriteIndexed(std::string filename) const
{
	// Test all properties that are auto-generated as correct when writing bitmap piecemeal
	if (imageHeader.compression != BmpCompression::Uncompressed) {
		throw std::runtime_error("Unable to write compressed bitmap files");
	}

	Validate();

	Stream::FileWriter fileWriter(filename);
	WriteIndexed(fileWriter);
}

void BitmapFile::WriteIndexed(Stream::Writer& writer) const
{
	VerifyIndexedImageForSerialization(imageHeader.bitCount);
	VerifyIndexedPaletteSizeDoesNotExceedBitCount(imageHeader.bitCount, palette.size());
	VerifyPixelSizeMatchesImageDimensionsWithPitch(imageHeader.bitCount, imageHeader.width, imageHeader.height, pixels.size());

	// Brett208: Should remove need to resize the palette to full length before writing to Stream
	auto paletteFullLength = palette;
	paletteFullLength.resize(ImageHeader::CalcMaxIndexedPaletteSize(imageHeader.bitCount), DiscreteColor::Black);

	WriteHeaders(writer, imageHeader.bitCount, imageHeader.width, imageHeader.height, palette);
	writer.Write(palette);

	WritePixels(writer, pixels, imageHeader.width, imageHeader.height, imageHeader.bitCount);
}

void BitmapFile::WriteHeaders(Stream::Writer& writer, uint16_t bitCount, int width, int height, const std::vector<Color>& palette)
{
	std::size_t pixelOffset = sizeof(BmpHeader) + sizeof(ImageHeader) + palette.size() * sizeof(Color);
	std::size_t fileSize = pixelOffset + ImageHeader::CalculatePitch(bitCount, width) * std::abs(height);

	if (fileSize > UINT32_MAX) {
		throw std::runtime_error("Bitmap size is too large to save to disk.");
	}

	auto bmpHeader = BmpHeader::Create(static_cast<uint32_t>(fileSize), static_cast<uint32_t>(pixelOffset));
	auto imageHeader = ImageHeader::Create(width, height, bitCount);

	writer.Write(bmpHeader);
	writer.Write(imageHeader);
}

void BitmapFile::WritePixels(Stream::Writer& writer, const std::vector<uint8_t>& pixels, int32_t width, int32_t height, uint16_t bitCount)
{
	// Bitmap height may be negative depending on format
	height = std::abs(height);

	const auto pitch = ImageHeader::CalculatePitch(bitCount, width);
	const auto bytesOfPixelsPerRow = ImageHeader::CalcPixelByteWidth(bitCount, width);
	const std::vector<uint8_t> padding(pitch - bytesOfPixelsPerRow, 0);

	for (int y = 0; y < height; ++y)
	{
		writer.Write(&pixels[y * pitch], bytesOfPixelsPerRow);
		writer.Write(padding);
	}
}
