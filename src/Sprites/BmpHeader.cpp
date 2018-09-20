#include "BmpHeader.h"

const std::array<char, 2> BmpHeader::defaultType{ 'B', 'M' };

BmpHeader BmpHeader::Create(uint32_t fileSize, uint32_t pixelOffset)
{
	return BmpHeader{ defaultType, fileSize, defaultReserved1, defaultReserved2, pixelOffset };
}
