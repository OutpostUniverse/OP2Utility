#include "ImageHeader.h"
#include <string>
#include <stdexcept>
#include <algorithm>
#include <cstring>

ImageHeader ImageHeader::Create(int32_t width, int32_t height, uint16_t bitCount)
{
	VerifyValidBitCount(bitCount);

	return ImageHeader{
		sizeof(ImageHeader),
		width,
		height,
		DefaultPlanes,
		bitCount,
		BmpCompression::Uncompressed,
		DefaultImageSize,
		DefaultXResolution,
		DefaultYResolution,
		DefaultUsedColorMapEntries,
		DefaultImportantColorCount };
}

const uint16_t ImageHeader::DefaultPlanes = 1;
const uint32_t ImageHeader::DefaultImageSize = 0;
const uint32_t ImageHeader::DefaultXResolution = 0;
const uint32_t ImageHeader::DefaultYResolution = 0;
const uint32_t ImageHeader::DefaultUsedColorMapEntries = 0;
const uint32_t ImageHeader::DefaultImportantColorCount = 0;

const std::array<uint16_t, 6> ImageHeader::ValidBitCounts{ 1, 4, 8, 16, 24, 32 };

bool ImageHeader::IsValidBitCount() const
{
	return ImageHeader::IsValidBitCount(bitCount);
}

bool ImageHeader::IsValidBitCount(uint16_t bitCount)
{
	return std::find(ValidBitCounts.begin(), ValidBitCounts.end(), bitCount) != ValidBitCounts.end();
}

bool ImageHeader::IsIndexedImage() const
{
	return ImageHeader::IsIndexedImage(bitCount);
}

bool ImageHeader::IsIndexedImage(uint16_t bitCount)
{
	return bitCount <= 8;
}

void ImageHeader::VerifyValidBitCount() const
{
	return ImageHeader::VerifyValidBitCount(bitCount);
}

void ImageHeader::VerifyValidBitCount(uint16_t bitCount)
{
	if (!IsValidBitCount(bitCount)) {
		throw std::runtime_error("A bit count of " + std::to_string(bitCount) + " is not supported");
	}
}

std::size_t ImageHeader::CalculatePitch() const
{
	return ImageHeader::CalculatePitch(bitCount, width);
}

std::size_t ImageHeader::CalculatePitch(uint16_t bitCount, int32_t width)
{
	const auto bytesOfPixelsPerRow = CalcPixelByteWidth(bitCount, width);
	return (bytesOfPixelsPerRow + 3) & ~3;
}

std::size_t ImageHeader::CalcPixelByteWidth() const
{
	return ImageHeader::CalcPixelByteWidth(bitCount, width);
}

std::size_t ImageHeader::CalcPixelByteWidth(uint16_t bitCount, int32_t width)
{
	const std::size_t bitsPerByte = 8;
	return ((width * bitCount) + (bitsPerByte - 1)) / bitsPerByte;
}

void ImageHeader::Validate() const
{
	if (headerSize != sizeof(ImageHeader)) {
		throw std::runtime_error("Image Header size must be equal to " + std::to_string(sizeof(ImageHeader)));
	}

	if (planes != DefaultPlanes) {
		throw std::runtime_error("Image format not supported: only single plane images are supported, but this image has " + std::to_string(planes));
	}

	VerifyValidBitCount(bitCount);

	if (usedColorMapEntries > CalcMaxIndexedPaletteSize()) {
		throw std::runtime_error("Used color map entries is greater than possible range of color map (palette)");
	}

	if (importantColorCount > CalcMaxIndexedPaletteSize()) {
		throw std::runtime_error("Important Color Count is greater than possible range of color map (palette)");
	}
}

std::size_t ImageHeader::CalcMaxIndexedPaletteSize() const {
	return CalcMaxIndexedPaletteSize(bitCount);
}

std::size_t ImageHeader::CalcMaxIndexedPaletteSize(uint16_t bitCount)
{
	if (!ImageHeader::IsIndexedImage(bitCount)) {
		throw std::runtime_error("Bit count does not have an associated max palette size");
	}

	return std::size_t{ 1 } << bitCount;
}

bool operator==(const ImageHeader& lhs, const ImageHeader& rhs) {
	return std::memcmp(&lhs, &rhs, sizeof(lhs)) == 0;
}

bool operator!=(const ImageHeader& lhs, const ImageHeader& rhs) {
	return !operator==(lhs, rhs);
}
