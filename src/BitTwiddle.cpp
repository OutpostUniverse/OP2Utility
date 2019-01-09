#include "BitTwiddle.h"


bool IsPowerOf2(uint32_t value)
{
	return value && !(value & (value - 1));
}

// This method assumes the argument is a power of 2
uint32_t Log2OfPowerOf2(uint32_t value)
{
	static const int MultiplyDeBruijnBitPosition2[32] =
	{
		0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
		31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
	};
	return MultiplyDeBruijnBitPosition2[static_cast<uint32_t>(value * 0x077CB531U) >> 27];
}
