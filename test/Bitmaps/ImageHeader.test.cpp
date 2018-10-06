#include "../src/Bitmaps/ImageHeader.h"
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
	EXPECT_EQ(ImageHeader::DefaultPlanes, imageHeader.DefaultPlanes);
	EXPECT_EQ(bitCount, imageHeader.bitCount);
	EXPECT_EQ(BmpCompression::Uncompressed, imageHeader.compression);
	EXPECT_EQ(ImageHeader::DefaultImageSize, imageHeader.DefaultImageSize);
	EXPECT_EQ(ImageHeader::DefaultXResolution, imageHeader.DefaultXResolution);
	EXPECT_EQ(ImageHeader::DefaultYResolution, imageHeader.DefaultYResolution);
	EXPECT_EQ(ImageHeader::DefaultUsedColorMapEntries, imageHeader.DefaultUsedColorMapEntries);
	EXPECT_EQ(ImageHeader::DefaultImportantColorCount, imageHeader.DefaultImportantColorCount);
}

TEST(ImageHeader, IsValidBitCount)
{
	for (auto bitCount : ImageHeader::ValidBitCounts) {
		EXPECT_TRUE(ImageHeader::IsValidBitCount(bitCount));
	}

	EXPECT_FALSE(ImageHeader::IsValidBitCount(21));
}

TEST(ImageHeader, IsIndexedImage)
{
	EXPECT_TRUE(ImageHeader::IsIndexedImage(2));
	EXPECT_TRUE(ImageHeader::IsIndexedImage(8));
	EXPECT_FALSE(ImageHeader::IsIndexedImage(16));
}

TEST(ImageHeader, VerifyValidBitCount)
{
	for (auto bitCount : ImageHeader::ValidBitCounts) {
		EXPECT_NO_THROW(ImageHeader::VerifyValidBitCount(bitCount));
	}
	
	EXPECT_THROW(ImageHeader::VerifyValidBitCount(0), std::runtime_error);
	EXPECT_THROW(ImageHeader::VerifyValidBitCount(3), std::runtime_error);
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
	imageHeader.planes = ImageHeader::DefaultPlanes;

	imageHeader.bitCount = 3;
	EXPECT_THROW(imageHeader.Validate(), std::runtime_error);
	imageHeader.bitCount = 1;

	imageHeader.usedColorMapEntries = 3;
	EXPECT_THROW(imageHeader.Validate(), std::runtime_error);
	imageHeader.usedColorMapEntries = ImageHeader::DefaultUsedColorMapEntries;

	imageHeader.importantColorCount = 3;
	EXPECT_THROW(imageHeader.Validate(), std::runtime_error);
	imageHeader.importantColorCount = ImageHeader::DefaultImportantColorCount;
}
