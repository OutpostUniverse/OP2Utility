#pragma once

#include "BmpCompression.h"
#include <cstdint>
#include <array>

#pragma pack(push, 1) // Make sure structures are byte aligned

struct ImageHeader
{
	static ImageHeader Create(int32_t width, int32_t height, uint16_t bitCount);

	uint32_t headerSize;
	int32_t width;
	int32_t height;
	uint16_t planes;
	uint16_t bitCount;
	BmpCompression compression;
	uint32_t imageSize; //Size in bytes of pixels. Valid to set to 0 if no compression used.
	uint32_t xResolution;
	uint32_t yResolution;
	uint32_t usedColorMapEntries;
	uint32_t importantColorCount;

	// Default values
	static const uint16_t defaultPlanes;
	static const uint32_t defaultImageSize;
	static const uint32_t defaultXResolution;
	static const uint32_t defaultYResolution;
	static const uint32_t defaultUsedColorMapEntries;
	static const uint32_t defaultImportantColorCount;

	// BitCount verification
	static const std::array<uint16_t, 6> validBitCounts;
	static const std::array<uint16_t, 3> indexedBitCounts;

	static bool IsValidBitCount(uint16_t bitCount);
	static bool IsIndexedBitCount(uint16_t bitCount);

	static void VerifyValidBitCount(uint16_t bitCount);
	static void VerifyIndexedBitCount(uint16_t bitCount);

	void Validate();
};

static_assert(40 == sizeof(ImageHeader), "ImageHeader is an unexpected size");

#pragma pack(pop)
