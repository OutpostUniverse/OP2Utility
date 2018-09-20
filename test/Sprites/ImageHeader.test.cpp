#include "../src/Sprites/ImageHeader.h"
#include <gtest/gtest.h>
#include <stdexcept>
#include <cstdint>

TEST(ImageHeader, Create)
{
	const int32_t width = 10;
	const int32_t height = 10;
	const uint16_t bitCount = 1;

	// Improper Bit Count throws
	EXPECT_THROW(ImageHeader::Create(width, height, 3), std::runtime_error);

	ImageHeader imageHeader;
	EXPECT_NO_THROW(imageHeader = ImageHeader::Create(width, height, bitCount));

	EXPECT_EQ(sizeof(ImageHeader), imageHeader.headerSize);
	EXPECT_EQ(width, imageHeader.width);
	EXPECT_EQ(height, imageHeader.height);
	EXPECT_EQ(ImageHeader::defaultPlanes, imageHeader.defaultPlanes);
	EXPECT_EQ(bitCount, imageHeader.bitCount);
	EXPECT_EQ(ImageHeader::defaultCompression, imageHeader.compression);
	EXPECT_EQ(ImageHeader::defaultImageSize, imageHeader.defaultImageSize);
	EXPECT_EQ(ImageHeader::defaultXResolution, imageHeader.defaultXResolution);
	EXPECT_EQ(ImageHeader::defaultYResolution, imageHeader.defaultYResolution);
	EXPECT_EQ(ImageHeader::defaultUsedColorMapEntries, imageHeader.defaultUsedColorMapEntries);
	EXPECT_EQ(ImageHeader::defaultImportantColorCount, imageHeader.defaultImportantColorCount);
}

TEST(ImageHeader, IsBitCount)
{
	for (auto bitCount : ImageHeader::validBitCounts) {
		EXPECT_TRUE(ImageHeader::IsBitCount(bitCount));
	}

	EXPECT_FALSE(ImageHeader::IsBitCount(21));
}

TEST(ImageHeader, IsIndexedBitCount)
{
	for (auto bitCount : ImageHeader::indexedBitCounts) {
		EXPECT_TRUE(ImageHeader::IsIndexedBitCount(bitCount));

		// Ensure Indexed Bit Count is also a valid general BitCount
		EXPECT_TRUE(ImageHeader::IsBitCount(bitCount));
	}

	EXPECT_FALSE(ImageHeader::IsIndexedBitCount(3));
}

TEST(ImageHeader, VerifyBitCount)
{
	for (auto bitCount : ImageHeader::validBitCounts) {
		EXPECT_NO_THROW(ImageHeader::VerifyBitCount(bitCount));
	}
	
	EXPECT_THROW(ImageHeader::VerifyBitCount(21), std::runtime_error);
}

TEST(ImageHeader, VerifyIndexedBitCount) 
{
	for (auto bitCount : ImageHeader::indexedBitCounts) {
		EXPECT_NO_THROW(ImageHeader::VerifyIndexedBitCount(bitCount));

		// Ensure Indexed Bit Count is also a valid general BitCount
		EXPECT_NO_THROW(ImageHeader::VerifyBitCount(bitCount));
	}

	EXPECT_THROW(ImageHeader::VerifyIndexedBitCount(3), std::runtime_error);
}
