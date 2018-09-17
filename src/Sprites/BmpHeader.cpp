#include "BmpHeader.h"

const std::array<char, 2> BmpHeader::headerBM{ 'B', 'M' };

BmpHeader BmpHeader::Create(uint32_t fileSize, uint32_t pixelOffset)
{
	return BmpHeader{ headerBM, fileSize, reserved1Default, reserved2Default, pixelOffset };
}
