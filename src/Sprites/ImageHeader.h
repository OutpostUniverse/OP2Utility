#pragma once

#include "BmpCompression.h"
#include <cstdint>
#include <array>

struct ImageHeader
{
	static const std::array<uint16_t, 6> validBitCounts;
	static const std::array<uint16_t, 3> indexedBitCounts;

	static bool IsValidBitCount(uint16_t bitCount);
	static bool IsIndexedBitCount(uint16_t bitCount);

	static void CheckValidBitCount(uint16_t bitCount);
	static void CheckIndexedBitCount(uint16_t bitCount);

	static ImageHeader Create(int32_t width, int32_t height, uint16_t bitCount)
	{
		CheckValidBitCount(bitCount);

		return ImageHeader{
			40,
			width,
			height,
			1,
			bitCount,
			BmpCompression::Uncompressed,
			0,
			0,
			0,
			0,
			0 };
	}

	uint32_t headerSize;
	int32_t width;
	int32_t height;
	uint16_t planes;
	uint16_t bitCount;
	BmpCompression compression;
	uint32_t imageSize;
	uint32_t xResolution;
	uint32_t yResolution;
	uint32_t usedColorMapEntries;
	uint32_t importantColorCount;
};
