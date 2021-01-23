#include "BitmapFile.h"
#include <stdexcept>
#include <cmath>

BitmapFile BitmapFile::CreateIndexed(uint16_t bitCount, uint32_t width, int32_t height)
{
	BitmapFile bitmapFile;
	bitmapFile.imageHeader = ImageHeader::Create(width, height, bitCount);
	bitmapFile.palette.resize(bitmapFile.imageHeader.CalcMaxIndexedPaletteSize());
	bitmapFile.pixels.resize(bitmapFile.imageHeader.CalculatePitch() * std::abs(height));

	const std::size_t pixelOffset = sizeof(BmpHeader) + sizeof(ImageHeader) + bitmapFile.palette.size() * sizeof(Color);
	const std::size_t bitmapFileSize = pixelOffset + bitmapFile.pixels.size() * sizeof(uint8_t);

	if (bitmapFileSize > UINT32_MAX) {
		throw std::runtime_error("Maximum size of a bitmap file has been exceeded");
	}

	bitmapFile.bmpHeader = BmpHeader::Create(static_cast<uint32_t>(bitmapFileSize), static_cast<uint32_t>(pixelOffset));

	return bitmapFile;
}

BitmapFile BitmapFile::CreateIndexed(uint16_t bitCount, uint32_t width, int32_t height, std::vector<Color> palette)
{
	if (palette.size() > std::size_t(1) << bitCount) {
		throw std::runtime_error("Unable to create bitmap. Provided palette length is greater than provided bit count.");
	}

	auto bitmapFile = BitmapFile::CreateIndexed(bitCount, width, height);
	std::move(palette.begin(), palette.end(), bitmapFile.palette.begin());

	return bitmapFile;
}

BitmapFile BitmapFile::CreateIndexed(uint16_t bitCount, uint32_t width, int32_t height, std::vector<Color> palette, std::vector<uint8_t> pixels)
{
	auto bitmap = CreateIndexed(bitCount, width, height, palette);
	bitmap.pixels = pixels;
	bitmap.VerifyPixelSizeMatchesImageDimensionsWithPitch();

	return bitmap;
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
		throw std::runtime_error("The size of pixels does not match the image's height times pitch");
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

ScanLineOrientation BitmapFile::ScanLineOrientation() const
{
	return imageHeader.height < 0 ? ScanLineOrientation::TopDown : ScanLineOrientation::BottomUp;
}

uint32_t BitmapFile::AbsoluteHeight() const
{
	return std::abs(imageHeader.height);
}

void BitmapFile::SwapRedAndBlue()
{
	for (auto& color : palette) {
		color.SwapRedAndBlue();
	}
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
