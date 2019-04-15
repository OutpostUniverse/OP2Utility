#include "../src/Bitmap/BitmapFile.h"
#include "Stream/DynamicMemoryWriter.h"
#include "XFile.h"
#include <gtest/gtest.h>
#include <stdexcept>

void RoundTripSub(uint16_t bitCount, int32_t width, int32_t height)
{
	const std::string filename("Sprite/data/BitmapTest.bmp");

	BitmapFile bitmapFile = BitmapFile::CreateDefaultIndexed(bitCount, width, height);
	BitmapFile bitmapFile2;

	EXPECT_NO_THROW(BitmapFile::WriteIndexed(filename, bitmapFile));
	EXPECT_NO_THROW(bitmapFile2 = BitmapFile::ReadIndexed(filename));
	EXPECT_EQ(bitmapFile, bitmapFile2);

	XFile::DeletePath(filename);
}

TEST(BitmapFile, RoundTripWriteAndRead)
{
	/* Test cases:
		 * Width below pitch
		 * Greater than 1 height
		 * Width equal to pitch
		 * Width above pitch
	*/
	{
		SCOPED_TRACE("Monochrome");
		RoundTripSub(1, 1, 1);
		RoundTripSub(1, 1, 2);
		RoundTripSub(1, 32, 1);
		RoundTripSub(1, 33, 1);
	}
	{
		SCOPED_TRACE("4 Bit");
		RoundTripSub(4, 1, 1);
		RoundTripSub(4, 1, 2);
		RoundTripSub(4, 8, 1);
		RoundTripSub(4, 9, 1);
	}
	{
		SCOPED_TRACE("8 Bit");
		RoundTripSub(8, 1, 1);
		RoundTripSub(8, 1, 2);
		RoundTripSub(8, 2, 1);
	}
}

TEST(BitmapFile, ReadFromMemory)
{
	Stream::DynamicMemoryWriter writer;
	auto bitmapFile = BitmapFile::CreateDefaultIndexed(1, 1, 1);
	//BitmapFile::WriteIndexed(writer);
}

TEST(BitmapFile, VerifyIndexedPaletteSizeDoesNotExceedBitCount)
{
	EXPECT_NO_THROW(BitmapFile::VerifyIndexedPaletteSizeDoesNotExceedBitCount(1, 1));
	EXPECT_NO_THROW(BitmapFile::VerifyIndexedPaletteSizeDoesNotExceedBitCount(1, 2));
	EXPECT_THROW(BitmapFile::VerifyIndexedPaletteSizeDoesNotExceedBitCount(1, 3), std::runtime_error);

	// Test non-static version of function
	BitmapFile bitmapFile = BitmapFile::CreateDefaultIndexed(1, 1, 1);
	EXPECT_NO_THROW(bitmapFile.VerifyIndexedPaletteSizeDoesNotExceedBitCount());
	bitmapFile.palette.resize(3);
	EXPECT_THROW(bitmapFile.VerifyIndexedPaletteSizeDoesNotExceedBitCount(), std::runtime_error);
}

TEST(BitmapFile, VerifyPixelSizeMatchesImageDimensionsWithPitch)
{
	EXPECT_NO_THROW(BitmapFile::VerifyPixelSizeMatchesImageDimensionsWithPitch(1, 1, 1, 4));
	EXPECT_THROW(BitmapFile::VerifyPixelSizeMatchesImageDimensionsWithPitch(1, 1, 1, 1), std::runtime_error);

	// Test non-static version of function
	BitmapFile bitmapFile = BitmapFile::CreateDefaultIndexed(1, 1, 1);;
	EXPECT_NO_THROW(bitmapFile.VerifyPixelSizeMatchesImageDimensionsWithPitch());
	bitmapFile.pixels.resize(1, 0);
	EXPECT_THROW(bitmapFile.VerifyPixelSizeMatchesImageDimensionsWithPitch(), std::runtime_error);
}

TEST(BitmapFile, Equality)
{
	BitmapFile bitmapFile1 = BitmapFile::CreateDefaultIndexed(1, 1, 1);	
	auto bitmapFile2 = bitmapFile1;

	EXPECT_TRUE(bitmapFile1 == bitmapFile2);
	EXPECT_FALSE(bitmapFile1 != bitmapFile2);

	bitmapFile1.pixels[0] = 1;
	EXPECT_FALSE(bitmapFile1 == bitmapFile2);
	EXPECT_TRUE(bitmapFile1 != bitmapFile2);
}
