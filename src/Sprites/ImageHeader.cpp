#include "ImageHeader.h"
#include <string>
#include <stdexcept>
#include <algorithm>

ImageHeader ImageHeader::Create(int32_t width, int32_t height, uint16_t bitCount)
{
	CheckValidBitCount(bitCount);

	return ImageHeader{
		sizeof(ImageHeader),
		width,
		height,
		defaultPlanes,
		bitCount,
		defaultCompression,
		defaultImageSize,
		defaultXResolution,
		defaultYResolution,
		defaultUsedColorMapEntries,
		defaultImportantColorCount };
}

const std::array<uint16_t, 6> ImageHeader::validBitCounts{ 1, 4, 8, 16, 24, 32 };
const std::array<uint16_t, 3> ImageHeader::indexedBitCounts{ 1, 4, 8 };

bool ImageHeader::IsValidBitCount(uint16_t bitCount)
{
	return std::find(validBitCounts.begin(), validBitCounts.end(), bitCount) != validBitCounts.end();
}

bool ImageHeader::IsIndexedBitCount(uint16_t bitCount)
{
	return std::find(indexedBitCounts.begin(), indexedBitCounts.end(), bitCount) != indexedBitCounts.end();
}

void ImageHeader::CheckValidBitCount(uint16_t bitCount)
{
	if (!IsValidBitCount(bitCount)) {
		throw std::runtime_error("A bit count of " + std::to_string(bitCount) + " is not supported");
	}
}

void ImageHeader::CheckIndexedBitCount(uint16_t bitCount)
{
	if (!IsIndexedBitCount(bitCount)) {
		throw std::runtime_error("A bit count of " + std::to_string(bitCount) + " does not support an indexed palette");
	}
}
