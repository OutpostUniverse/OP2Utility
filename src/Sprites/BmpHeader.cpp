#include "BmpHeader.h"
#include <stdexcept>

const std::array<char, 2> BmpHeader::defaultFileSignature{ 'B', 'M' };
const uint16_t BmpHeader::defaultReserved1 = 0;
const uint16_t BmpHeader::defaultReserved2 = 0;

BmpHeader BmpHeader::Create(uint32_t fileSize, uint32_t pixelOffset)
{
	return BmpHeader{ defaultFileSignature, fileSize, defaultReserved1, defaultReserved2, pixelOffset };
}

bool BmpHeader::IsValidFileSignature()
{
	return fileSignature == defaultFileSignature;
}

void BmpHeader::VerifyFileSignature()
{
	if (!IsValidFileSignature()) {
		throw std::runtime_error("BmpHeader does not contain a proper File Signature (Magic Number).");
	}
}
