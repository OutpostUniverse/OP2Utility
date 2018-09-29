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
	EXPECT_EQ(BmpCompression::Uncompressed, imageHeader.compression);
	EXPECT_EQ(ImageHeader::defaultImageSize, imageHeader.defaultImageSize);
	EXPECT_EQ(ImageHeader::defaultXResolution, imageHeader.defaultXResolution);
	EXPECT_EQ(ImageHeader::defaultYResolution, imageHeader.defaultYResolution);
	EXPECT_EQ(ImageHeader::defaultUsedColorMapEntries, imageHeader.defaultUsedColorMapEntries);
	EXPECT_EQ(ImageHeader::defaultImportantColorCount, imageHeader.defaultImportantColorCount);
}

TEST(ImageHeader, IsValidBitCount)
{
	for (auto bitCount : ImageHeader::validBitCounts) {
		EXPECT_TRUE(ImageHeader::IsValidBitCount(bitCount));
	}

	EXPECT_FALSE(ImageHeader::IsValidBitCount(21));
}

TEST(ImageHeader, IsIndexedBitCount)
{
	for (auto bitCount : ImageHeader::indexedBitCounts) {
		EXPECT_TRUE(ImageHeader::IsIndexedBitCount(bitCount));

		// Ensure Indexed Bit Count is also a valid general BitCount
		EXPECT_TRUE(ImageHeader::IsValidBitCount(bitCount));
	}

	EXPECT_FALSE(ImageHeader::IsIndexedBitCount(3));
}

TEST(ImageHeader, VerifyValidBitCount)
{
	for (auto bitCount : ImageHeader::validBitCounts) {
		EXPECT_NO_THROW(ImageHeader::VerifyValidBitCount(bitCount));
	}
	
	EXPECT_THROW(ImageHeader::VerifyValidBitCount(0), std::runtime_error);
	EXPECT_THROW(ImageHeader::VerifyValidBitCount(3), std::runtime_error);
}

TEST(ImageHeader, VerifyIndexedBitCount) 
{
	for (auto bitCount : ImageHeader::indexedBitCounts) {
		EXPECT_NO_THROW(ImageHeader::VerifyIndexedBitCount(bitCount));

		// Ensure Indexed Bit Count is also a valid general BitCount
		EXPECT_NO_THROW(ImageHeader::VerifyValidBitCount(bitCount));
	}

	EXPECT_THROW(ImageHeader::VerifyIndexedBitCount(0), std::runtime_error);
	EXPECT_THROW(ImageHeader::VerifyIndexedBitCount(3), std::runtime_error);
}

TEST(ImageHeader, Validate)
{
	ImageHeader imageHeader = ImageHeader::Create(1, 1, 1);

	EXPECT_NO_THROW(imageHeader.Validate());

	imageHeader.headerSize = 0;
	EXPECT_THROW(imageHeader.Validate(), std::runtime_error);
	imageHeader.headerSize = sizeof(ImageHeader);

	imageHeader.planes = 0;
	EXPECT_THROW(imageHeader.Validate(), std::runtime_error);
	imageHeader.planes = ImageHeader::defaultPlanes;

	imageHeader.bitCount = 3;
	EXPECT_THROW(imageHeader.Validate(), std::runtime_error);
	imageHeader.bitCount = 1;

	imageHeader.compression = static_cast<BmpCompression>(14);
	EXPECT_THROW(imageHeader.Validate(), std::runtime_error);
	imageHeader.compression = BmpCompression::Uncompressed;

	imageHeader.usedColorMapEntries = 3;
	EXPECT_THROW(imageHeader.Validate(), std::runtime_error);
	imageHeader.usedColorMapEntries = ImageHeader::defaultUsedColorMapEntries;

	imageHeader.importantColorCount = 3;
	EXPECT_THROW(imageHeader.Validate(), std::runtime_error);
	imageHeader.importantColorCount = ImageHeader::defaultImportantColorCount;
}
