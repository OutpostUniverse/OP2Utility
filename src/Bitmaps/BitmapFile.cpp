#include "BitmapFile.h"
#include <stdexcept>
#include <cmath>

void BitmapFile::VerifyIndexedPaletteSizeDoesNotExceedBitCount() const
{
	return BitmapFile::VerifyIndexedPaletteSizeDoesNotExceedBitCount(imageHeader.bitCount, palette.size());
}

void BitmapFile::VerifyIndexedPaletteSizeDoesNotExceedBitCount(uint16_t bitCount, std::size_t paletteSize)
{
	if (paletteSize > std::size_t{ 1 } << bitCount) {
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
