#include "BitmapFile.h"
#include "../Stream/FileReader.h"
#include <stdexcept>

BitmapFile BitmapFile::ReadIndexed(const std::string& filename)
{
	Stream::FileReader fileReader(filename);
	return ReadIndexed(fileReader);
}

BitmapFile BitmapFile::ReadIndexed(Stream::ForwardReader& reader)
{
	BitmapFile bitmapFile;
	bitmapFile.bmpHeader = ReadBmpHeader(reader);
	bitmapFile.imageHeader = ReadImageHeader(reader);

	ReadPalette(reader, bitmapFile);
	ReadPixels(reader, bitmapFile);

	return bitmapFile;
}

// Read indexed bitmap from an rvalue stream (unnamed temporary)
BitmapFile BitmapFile::ReadIndexed(Stream::ForwardReader&& reader)
{
	// Delegate to lvalue overload
	return ReadIndexed(reader);
}

BmpHeader BitmapFile::ReadBmpHeader(Stream::ForwardReader& reader)
{
	BmpHeader bmpHeader;
	reader.Read(bmpHeader);

	bmpHeader.VerifyFileSignature();

	if (bmpHeader.size < reader.Length()) {
		throw std::runtime_error("Bitmap file size exceed length of stream.");
	}

	return bmpHeader;
}

ImageHeader BitmapFile::ReadImageHeader(Stream::Reader& reader)
{
	ImageHeader imageHeader;
	reader.Read(imageHeader);

	imageHeader.Validate();

	VerifyIndexedImageForSerialization(imageHeader.bitCount);

	return imageHeader;
}

void BitmapFile::ReadPalette(Stream::Reader& reader, BitmapFile& bitmapFile)
{
	bitmapFile.palette.clear();

	if (bitmapFile.imageHeader.usedColorMapEntries != 0) {
		bitmapFile.palette.resize(bitmapFile.imageHeader.usedColorMapEntries);
	}
	else {
		bitmapFile.palette.resize(bitmapFile.imageHeader.CalcMaxIndexedPaletteSize());
	}

	reader.Read(bitmapFile.palette);
}

void BitmapFile::ReadPixels(Stream::Reader& reader, BitmapFile& bitmapFile)
{
	std::size_t pixelContainerSize = bitmapFile.bmpHeader.size - bitmapFile.bmpHeader.pixelOffset;
	BitmapFile::VerifyPixelSizeMatchesImageDimensionsWithPitch(bitmapFile.imageHeader.bitCount, bitmapFile.imageHeader.width, bitmapFile.imageHeader.height, pixelContainerSize);

	bitmapFile.pixels.clear();
	bitmapFile.pixels.resize(pixelContainerSize);
	reader.Read(bitmapFile.pixels);
}
