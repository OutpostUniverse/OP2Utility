#include "BmpHeader.h"
#include <stdexcept>

const std::array<char, 2> BmpHeader::FileSignature{ 'B', 'M' };
const uint16_t BmpHeader::DefaultReserved1 = 0;
const uint16_t BmpHeader::DefaultReserved2 = 0;

BmpHeader BmpHeader::Create(uint32_t fileSize, uint32_t pixelOffset)
{
	return BmpHeader{ FileSignature, fileSize, DefaultReserved1, DefaultReserved2, pixelOffset };
}

bool BmpHeader::IsValidFileSignature() const
{
	return fileSignature == FileSignature;
}

void BmpHeader::VerifyFileSignature() const
{
	if (!IsValidFileSignature()) {
		throw std::runtime_error("BmpHeader does not contain a proper File Signature (Magic Number).");
	}
}

bool operator==(const BmpHeader& lhs, const BmpHeader& rhs) {
	return std::memcmp(&lhs, &rhs, sizeof(lhs)) == 0;
}

bool operator!=(const BmpHeader& lhs, const BmpHeader& rhs) {
	return !operator==(lhs, rhs);
}
