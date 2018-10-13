#include "BitmapFile.h"
#include <stdexcept>
#include <cmath>

BitmapFile BitmapFile::CreateDefaultIndexed(uint16_t bitCount, uint32_t width, uint32_t height)
{
	BitmapFile bitmapFile;
	bitmapFile.imageHeader = ImageHeader::Create(width, height, bitCount);
	bitmapFile.palette.resize(bitmapFile.imageHeader.CalcMaxIndexedPaletteSize());
	bitmapFile.pixels.resize(bitmapFile.imageHeader.CalculatePitch() * height);

	const std::size_t pixelOffset = sizeof(BmpHeader) + sizeof(ImageHeader) + bitmapFile.palette.size() * sizeof(Color);
	const std::size_t bitmapFileSize = pixelOffset + bitmapFile.pixels.size() * sizeof(uint8_t);

	if (bitmapFileSize > UINT32_MAX) {
		throw std::runtime_error("Maximum size of a bitmap file has been exceeded");
	}

	bitmapFile.bmpHeader = BmpHeader::Create(static_cast<uint32_t>(bitmapFileSize), static_cast<uint32_t>(pixelOffset));

	return bitmapFile;
}

void BitmapFile::VerifyIndexedPaletteSizeDoesNotExceedBitCount() const
{
	return BitmapFile::VerifyIndexedPaletteSizeDoesNotExceedBitCount(imageHeader.bitCount, palette.size());
}

void BitmapFile::VerifyIndexedPaletteSizeDoesNotExceedBitCount(uint16_t bitCount, std::size_t paletteSize)
{
	if (paletteSize > ImageHeader::CalcMaxIndexedPaletteSize(bitCount)) {
		throw std::runtime_error("Too many colors listed on the indexed palette");
	}
}

void BitmapFile::VerifyPixelSizeMatchesImageDimensionsWithPitch() const
{
	BitmapFile::VerifyPixelSizeMatchesImageDimensionsWithPitch(imageHeader.bitCount, imageHeader.width, imageHeader.height, pixels.size());
}

void BitmapFile::VerifyPixelSizeMatchesImageDimensionsWithPitch(uint16_t bitCount, int32_t width, int32_t height, std::size_t pixelsWithPitchSize)
{
	if (pixelsWithPitchSize != ImageHeader::CalculatePitch(bitCount, width) * std::abs(height)) {
		throw std::runtime_error("The size of pixels does not match the image's height time pitch");
	}
}

void BitmapFile::VerifyIndexedImageForSerialization(uint16_t bitCount)
{
	if (!ImageHeader::IsIndexedImage(bitCount)) {
		throw std::runtime_error("Unable to read/write a non-indexed bitmap file. Bit count is " + std::to_string(bitCount) + " but must be 8 or less");
	}
}

void BitmapFile::Validate() const
{
	bmpHeader.VerifyFileSignature();
	imageHeader.Validate();

	VerifyIndexedPaletteSizeDoesNotExceedBitCount();
	VerifyPixelSizeMatchesImageDimensionsWithPitch();
}

bool operator==(const BitmapFile& lhs, const BitmapFile& rhs) {
	return lhs.bmpHeader == rhs.bmpHeader && 
		lhs.imageHeader == rhs.imageHeader && 
		lhs.palette == rhs.palette &&
		lhs.pixels == rhs.pixels;
}

bool operator!=(const BitmapFile& lhs, const BitmapFile& rhs) {
	return !operator==(lhs, rhs);
}
