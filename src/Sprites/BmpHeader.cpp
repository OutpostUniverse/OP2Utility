#include "BmpHeader.h"

const std::array<char, 2> BmpHeader::defaultFileSignature{ 'B', 'M' };
const uint16_t BmpHeader::defaultReserved1 = 0;
const uint16_t BmpHeader::defaultReserved2 = 0;

BmpHeader BmpHeader::Create(uint32_t fileSize, uint32_t pixelOffset)
{
	return BmpHeader{ defaultFileSignature, fileSize, defaultReserved1, defaultReserved2, pixelOffset };
}
