#pragma once

#include <cstdint>

enum class BmpCompression : uint32_t
{
	Uncompressed,
	RLE8,
	RLE4,
	Bitfields
};
