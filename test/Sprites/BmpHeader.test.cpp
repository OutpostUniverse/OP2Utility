#include "../src/Sprites/BmpHeader.h"
#include <gtest/gtest.h>

TEST(BmpHeader, Create)
{
	const uint32_t fileSize = 100;
	const uint32_t pixelOffset = 50;

	BmpHeader bmpHeader;
	EXPECT_NO_THROW(bmpHeader = BmpHeader::Create(fileSize, pixelOffset));

	EXPECT_EQ(BmpHeader::defaultType, bmpHeader.type);
	EXPECT_EQ(fileSize, bmpHeader.size);
	EXPECT_EQ(BmpHeader::defaultReserved1, bmpHeader.reserved1);
	EXPECT_EQ(BmpHeader::defaultReserved2, bmpHeader.reserved2);
	EXPECT_EQ(pixelOffset, bmpHeader.pixelOffset);
}
