#include "BmpHeader.h"

const std::array<char, 2> BmpHeader::defaultHeaderType{ 'B', 'M' };

BmpHeader BmpHeader::Create(uint32_t fileSize, uint32_t pixelOffset)
{
	return BmpHeader{ defaultHeaderType, fileSize, defaultReserved1, defaultReserved2, pixelOffset };
}
