#pragma once

#include <array>
#include <cstdint>

#pragma pack(push, 1) // Make sure structures are byte aligned

struct BmpHeader
{
	// @pixelOffset: Offset from start of file to first pixel in image
	static BmpHeader Create(uint32_t fileSize, uint32_t pixelOffset);

	std::array<char, 2> type;
	uint32_t size;
	uint16_t reserved1;
	uint16_t reserved2;
	uint32_t pixelOffset;

	static const std::array<char, 2> defaultType;
	static const uint16_t defaultReserved1;
	static const uint16_t defaultReserved2;
};

#pragma pack(pop)
