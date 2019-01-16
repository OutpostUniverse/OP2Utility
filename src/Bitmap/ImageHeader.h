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
	static const uint16_t DefaultPlanes;
	static const uint32_t DefaultImageSize;
	static const uint32_t DefaultXResolution;
	static const uint32_t DefaultYResolution;
	static const uint32_t DefaultUsedColorMapEntries;
	static const uint32_t DefaultImportantColorCount;

	// BitCount verification
	static const std::array<uint16_t, 6> ValidBitCounts;

	bool IsValidBitCount() const;
	static bool IsValidBitCount(uint16_t bitCount);

	bool IsIndexedImage() const;
	static bool IsIndexedImage(uint16_t bitCount);

	void VerifyValidBitCount() const;
	static void VerifyValidBitCount(uint16_t bitCount);

	std::size_t CalculatePitch() const;
	static std::size_t CalculatePitch(uint16_t bitCount, int32_t width);

	// Does not include padding
	std::size_t CalcPixelByteWidth() const;
	static std::size_t CalcPixelByteWidth(uint16_t bitCount, int32_t width);

	std::size_t CalcMaxIndexedPaletteSize() const;
	static std::size_t CalcMaxIndexedPaletteSize(uint16_t bitCount);

	void Validate() const;
};

static_assert(40 == sizeof(ImageHeader), "ImageHeader is an unexpected size");

#pragma pack(pop)

bool operator==(const ImageHeader& lhs, const ImageHeader& rhs);
bool operator!=(const ImageHeader& lhs, const ImageHeader& rhs);
