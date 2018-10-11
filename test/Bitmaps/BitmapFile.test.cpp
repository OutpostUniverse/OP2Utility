#include "../src/Bitmaps/BitmapFile.h"
#include <gtest/gtest.h>
#include <stdexcept>

TEST(BitmapFile, VerifyIndexedPaletteSizeDoesNotExceedBitCount)
{
	EXPECT_NO_THROW(BitmapFile::VerifyIndexedPaletteSizeDoesNotExceedBitCount(1, 1));
	EXPECT_NO_THROW(BitmapFile::VerifyIndexedPaletteSizeDoesNotExceedBitCount(1, 2));
	EXPECT_THROW(BitmapFile::VerifyIndexedPaletteSizeDoesNotExceedBitCount(1, 3), std::runtime_error);

	// Test non-static version of function
	BitmapFile bitmapFile;
	bitmapFile.imageHeader = ImageHeader::Create(1, 1, 1);
	bitmapFile.palette.resize(2);
	EXPECT_NO_THROW(bitmapFile.VerifyIndexedPaletteSizeDoesNotExceedBitCount());
	bitmapFile.palette.resize(3);
	EXPECT_THROW(bitmapFile.VerifyIndexedPaletteSizeDoesNotExceedBitCount(), std::runtime_error);
}

TEST(BitmapFile, VerifyPixelSizeMatchesImageDimensionsWithPitch)
{
	EXPECT_NO_THROW(BitmapFile::VerifyPixelSizeMatchesImageDimensionsWithPitch(1, 1, 1, 4));
	EXPECT_THROW(BitmapFile::VerifyPixelSizeMatchesImageDimensionsWithPitch(1, 1, 1, 1), std::runtime_error);

	// Test non-static version of function
	BitmapFile bitmapFile;
	bitmapFile.imageHeader = ImageHeader::Create(1, 1, 1);
	bitmapFile.pixels.resize(4, 0);
	EXPECT_NO_THROW(bitmapFile.VerifyPixelSizeMatchesImageDimensionsWithPitch());
	bitmapFile.pixels.resize(1, 0);
	EXPECT_THROW(bitmapFile.VerifyPixelSizeMatchesImageDimensionsWithPitch(), std::runtime_error);
}

TEST(BitmapFile, Equality)
{
	BitmapFile bitmapFile1;
	bitmapFile1.bmpHeader = BmpHeader::Create(1, 1);
	bitmapFile1.imageHeader = ImageHeader::Create(1, 1, 1);
	bitmapFile1.palette.resize(2, DiscreteColor::Black);
	bitmapFile1.pixels.resize(1, 0);
	
	auto bitmapFile2 = bitmapFile1;

	EXPECT_TRUE(bitmapFile1 == bitmapFile2);
	EXPECT_FALSE(bitmapFile1 != bitmapFile2);

	bitmapFile1.pixels[0] = 1;
	EXPECT_FALSE(bitmapFile1 == bitmapFile2);
	EXPECT_TRUE(bitmapFile1 != bitmapFile2);
}
