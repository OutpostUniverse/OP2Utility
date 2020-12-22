#include "../src/Bitmap/ImageHeader.h"
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

	EXPECT_FALSE(ImageHeader::IsValidBitCount(3));

	// Test non-static version of function
	ImageHeader imageHeader;
	imageHeader.bitCount = 1;
	EXPECT_TRUE(imageHeader.IsValidBitCount());
	imageHeader.bitCount = 3;
	EXPECT_FALSE(imageHeader.IsValidBitCount());
}

TEST(ImageHeader, IsIndexedImage)
{
	EXPECT_TRUE(ImageHeader::IsIndexedImage(1));
	EXPECT_TRUE(ImageHeader::IsIndexedImage(8));
	EXPECT_FALSE(ImageHeader::IsIndexedImage(16));

	// Test non-static version of function
	ImageHeader imageHeader;
	imageHeader.bitCount = 1;
	EXPECT_TRUE(imageHeader.IsIndexedImage());
	imageHeader.bitCount = 16;
	EXPECT_FALSE(imageHeader.IsIndexedImage());
}

TEST(ImageHeader, VerifyValidBitCount)
{
	for (auto bitCount : ImageHeader::ValidBitCounts) {
		EXPECT_NO_THROW(ImageHeader::VerifyValidBitCount(bitCount));
	}
	
	EXPECT_THROW(ImageHeader::VerifyValidBitCount(0), std::runtime_error);
	EXPECT_THROW(ImageHeader::VerifyValidBitCount(3), std::runtime_error);

	// Test non-static version of function
	ImageHeader imageHeader;
	imageHeader.bitCount = 1;
	EXPECT_NO_THROW(imageHeader.VerifyValidBitCount());
	imageHeader.bitCount = 3;
	EXPECT_THROW(imageHeader.VerifyValidBitCount(), std::runtime_error);
}

TEST(ImageHeader, CalculatePitch)
{
	EXPECT_EQ(4u, ImageHeader::CalculatePitch(1, 1));
	EXPECT_EQ(4u, ImageHeader::CalculatePitch(1, 32));
	EXPECT_EQ(8u, ImageHeader::CalculatePitch(1, 33));

	EXPECT_EQ(4u, ImageHeader::CalculatePitch(4, 1));
	EXPECT_EQ(4u, ImageHeader::CalculatePitch(4, 8));
	EXPECT_EQ(8u, ImageHeader::CalculatePitch(4, 9));

	EXPECT_EQ(4u, ImageHeader::CalculatePitch(8, 1));
	EXPECT_EQ(4u, ImageHeader::CalculatePitch(8, 4));
	EXPECT_EQ(8u, ImageHeader::CalculatePitch(8, 5));

	// Test non-static version of function
	ImageHeader imageHeader;
	imageHeader.bitCount = 1;
	imageHeader.width = 1;
	EXPECT_EQ(4u, imageHeader.CalculatePitch());
}

TEST(ImageHeader, CalcByteWidth)
{
	EXPECT_EQ(1u, ImageHeader::CalcPixelByteWidth(1, 1));
	EXPECT_EQ(1u, ImageHeader::CalcPixelByteWidth(1, 8));
	EXPECT_EQ(2u, ImageHeader::CalcPixelByteWidth(1, 9));

	EXPECT_EQ(1u, ImageHeader::CalcPixelByteWidth(4, 1));
	EXPECT_EQ(1u, ImageHeader::CalcPixelByteWidth(4, 2));
	EXPECT_EQ(2u, ImageHeader::CalcPixelByteWidth(4, 3));

	EXPECT_EQ(1u, ImageHeader::CalcPixelByteWidth(8, 1));
	EXPECT_EQ(2u, ImageHeader::CalcPixelByteWidth(8, 2));

	// Test non-static version of function
	ImageHeader imageHeader;
	imageHeader.bitCount = 1;
	imageHeader.width = 1;
	EXPECT_EQ(1u, imageHeader.CalcPixelByteWidth());
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

TEST(ImageHeader, Equality)
{
	auto imageHeader1 = ImageHeader::Create(1, 1, 1);
	auto imageHeader2 = ImageHeader::Create(1, 1, 1);

	EXPECT_TRUE(imageHeader1 == imageHeader2);
	EXPECT_FALSE(imageHeader1 != imageHeader2);

	imageHeader2.bitCount = 8;
	EXPECT_FALSE(imageHeader1 == imageHeader2);
	EXPECT_TRUE(imageHeader1 != imageHeader2);
}
