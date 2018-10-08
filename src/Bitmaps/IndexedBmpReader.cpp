#include "BitmapFile.h"
#include "../Streams/FileReader.h"
#include <stdexcept>

BitmapFile BitmapFile::ReadIndexed(const std::string& filename)
{
	Stream::FileReader fileReader(filename);
	return ReadIndexed(fileReader);
}

BitmapFile BitmapFile::ReadIndexed(Stream::SeekableReader& seekableReader)
{
	BitmapFile bitmapFile;
	bitmapFile.bmpHeader = ReadBmpHeader(seekableReader);
	bitmapFile.imageHeader = ReadImageHeader(seekableReader);

	ReadPalette(seekableReader, bitmapFile);
	ReadPixels(seekableReader, bitmapFile);

	return bitmapFile;
}

BmpHeader BitmapFile::ReadBmpHeader(Stream::SeekableReader& seekableReader)
{
	BmpHeader bmpHeader;
	seekableReader.Read(bmpHeader);

	bmpHeader.VerifyFileSignature();

	if (bmpHeader.size < seekableReader.Length()) {
		throw std::runtime_error("Bitmap file size exceed length of stream.");
	}

	return bmpHeader;
}

ImageHeader BitmapFile::ReadImageHeader(Stream::SeekableReader& seekableReader)
{
	ImageHeader imageHeader;
	seekableReader.Read(imageHeader);

	imageHeader.Validate();

	VerifyIndexedImageForSerialization(imageHeader.bitCount);

	return imageHeader;
}

void BitmapFile::ReadPalette(Stream::SeekableReader& seekableReader, BitmapFile& bitmapFile)
{
	bitmapFile.palette.clear();

	if (bitmapFile.imageHeader.usedColorMapEntries != 0) {
		bitmapFile.palette.resize(bitmapFile.imageHeader.usedColorMapEntries);
	}
	else {
		bitmapFile.palette.resize(std::size_t{ 1 } << bitmapFile.imageHeader.bitCount);
	}

	seekableReader.Read(bitmapFile.palette);
}

void BitmapFile::ReadPixels(Stream::SeekableReader& seekableReader, BitmapFile& bitmapFile)
{
	std::size_t pixelContainerSize = bitmapFile.bmpHeader.size - bitmapFile.bmpHeader.pixelOffset;
	BitmapFile::VerifyPixelSizeMatchesImageDimensionsWithPitch(bitmapFile.imageHeader.bitCount, bitmapFile.imageHeader.width, bitmapFile.imageHeader.height, pixelContainerSize);

	bitmapFile.pixels.clear();
	bitmapFile.pixels.resize(pixelContainerSize);
	seekableReader.Read(bitmapFile.pixels);
}
