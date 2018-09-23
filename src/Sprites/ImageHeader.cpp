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
		defaultPlanes,
		bitCount,
		BmpCompression::Uncompressed,
		defaultImageSize,
		defaultXResolution,
		defaultYResolution,
		defaultUsedColorMapEntries,
		defaultImportantColorCount };
}

const uint16_t ImageHeader::defaultPlanes = 1;
const uint32_t ImageHeader::defaultImageSize = 0;
const uint32_t ImageHeader::defaultXResolution = 0;
const uint32_t ImageHeader::defaultYResolution = 0;
const uint32_t ImageHeader::defaultUsedColorMapEntries = 0;
const uint32_t ImageHeader::defaultImportantColorCount = 0;

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
