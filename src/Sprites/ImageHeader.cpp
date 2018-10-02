#include "ImageHeader.h"
#include <string>
#include <stdexcept>
#include <algorithm>

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
const std::array<uint16_t, 3> ImageHeader::IndexedBitCounts{ 1, 4, 8 };

bool ImageHeader::IsValidBitCount(uint16_t bitCount)
{
	return std::find(ValidBitCounts.begin(), ValidBitCounts.end(), bitCount) != ValidBitCounts.end();
}

bool ImageHeader::IsIndexedBitCount(uint16_t bitCount)
{
	return std::find(IndexedBitCounts.begin(), IndexedBitCounts.end(), bitCount) != IndexedBitCounts.end();
}

void ImageHeader::VerifyValidBitCount(uint16_t bitCount)
{
	if (!IsValidBitCount(bitCount)) {
		throw std::runtime_error("A bit count of " + std::to_string(bitCount) + " is not supported");
	}
}

void ImageHeader::VerifyIndexedBitCount(uint16_t bitCount)
{
	if (!IsIndexedBitCount(bitCount)) {
		throw std::runtime_error("A bit count of " + std::to_string(bitCount) + " does not support an indexed palette");
	}
}

void ImageHeader::Validate()
{
	if (headerSize != sizeof(ImageHeader)) {
		throw std::runtime_error("Image Header size must be equal to " + std::to_string(sizeof(ImageHeader)));
	}

	if (planes != DefaultPlanes) {
		throw std::runtime_error("Image format not supported: only single plane images are supported, but this image has " + std::to_string(planes));
	}

	VerifyValidBitCount(bitCount);

	if (usedColorMapEntries > std::size_t{ 1 } << bitCount) {
		throw std::runtime_error("Used color map entries is greater than possible range of color map (palette)");
	}

	if (importantColorCount > std::size_t{ 1 } << bitCount) {
		throw std::runtime_error("Important Color Count is greater than possible range of color map (palette)");
	}
}
