#include "BitmapFile.h"
#include "../Stream/FileReader.h"
#include <stdexcept>

BitmapFile BitmapFile::ReadIndexed(const std::string& filename)
{
	Stream::FileReader fileReader(filename);
	return ReadIndexed(fileReader);
}

BitmapFile BitmapFile::ReadIndexed(Stream::BidirectionalReader& seekableReader)
{
	BitmapFile bitmapFile;
	bitmapFile.bmpHeader = ReadBmpHeader(seekableReader);
	bitmapFile.imageHeader = ReadImageHeader(seekableReader);

	ReadPalette(seekableReader, bitmapFile);
	ReadPixels(seekableReader, bitmapFile);

	return bitmapFile;
}

BmpHeader BitmapFile::ReadBmpHeader(Stream::BidirectionalReader& seekableReader)
{
	BmpHeader bmpHeader;
	seekableReader.Read(bmpHeader);

	bmpHeader.VerifyFileSignature();

	if (bmpHeader.size < seekableReader.Length()) {
		throw std::runtime_error("Bitmap file size exceed length of stream.");
	}

	return bmpHeader;
}

ImageHeader BitmapFile::ReadImageHeader(Stream::BidirectionalReader& seekableReader)
{
	ImageHeader imageHeader;
	seekableReader.Read(imageHeader);

	imageHeader.Validate();

	VerifyIndexedImageForSerialization(imageHeader.bitCount);

	return imageHeader;
}

void BitmapFile::ReadPalette(Stream::BidirectionalReader& seekableReader, BitmapFile& bitmapFile)
{
	bitmapFile.palette.clear();

	if (bitmapFile.imageHeader.usedColorMapEntries != 0) {
		bitmapFile.palette.resize(bitmapFile.imageHeader.usedColorMapEntries);
	}
	else {
		bitmapFile.palette.resize(bitmapFile.imageHeader.CalcMaxIndexedPaletteSize());
	}

	seekableReader.Read(bitmapFile.palette);
}

void BitmapFile::ReadPixels(Stream::BidirectionalReader& seekableReader, BitmapFile& bitmapFile)
{
	std::size_t pixelContainerSize = bitmapFile.bmpHeader.size - bitmapFile.bmpHeader.pixelOffset;
	BitmapFile::VerifyPixelSizeMatchesImageDimensionsWithPitch(bitmapFile.imageHeader.bitCount, bitmapFile.imageHeader.width, bitmapFile.imageHeader.height, pixelContainerSize);

	bitmapFile.pixels.clear();
	bitmapFile.pixels.resize(pixelContainerSize);
	seekableReader.Read(bitmapFile.pixels);
}
