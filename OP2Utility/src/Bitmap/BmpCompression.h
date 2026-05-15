#pragma once

#include <cstdint>

namespace OP2Utility
{
	enum class BmpCompression : uint32_t
	{
		Uncompressed,
		RLE8,
		RLE4,
		Bitfields
	};
}
