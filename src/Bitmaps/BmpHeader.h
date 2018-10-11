#pragma once

#include <array>
#include <cstdint>
#include <cstring>

#pragma pack(push, 1) // Make sure structures are byte aligned

struct BmpHeader
{
	// @pixelOffset: Offset from start of file to first pixel in image
	static BmpHeader Create(uint32_t fileSize, uint32_t pixelOffset);

	std::array<char, 2> fileSignature;
	uint32_t size;
	uint16_t reserved1;
	uint16_t reserved2;
	uint32_t pixelOffset;

	static const std::array<char, 2> FileSignature;
	static const uint16_t DefaultReserved1;
	static const uint16_t DefaultReserved2;

	bool IsValidFileSignature() const;
	void VerifyFileSignature() const;
};

static_assert(14 == sizeof(BmpHeader), "BmpHeader is an unexpected size");

#pragma pack(pop)

bool operator==(const BmpHeader& lhs, const BmpHeader& rhs);
bool operator!=(const BmpHeader& lhs, const BmpHeader& rhs);
