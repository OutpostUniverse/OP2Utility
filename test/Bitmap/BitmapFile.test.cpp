#include "../src/Bitmap/BitmapFile.h"
#include "../src/Stream/DynamicMemoryWriter.h"
#include "XFile.h"
#include <gtest/gtest.h>
#include <stdexcept>

void WriteAndReadBitmapSub(uint16_t bitCount, int32_t width, int32_t height)
{
	Stream::DynamicMemoryWriter writer;

	BitmapFile bitmapFile = BitmapFile::CreateIndexed(bitCount, width, height);
	BitmapFile bitmapFile2;

	EXPECT_NO_THROW(bitmapFile.WriteIndexed(writer));
	EXPECT_NO_THROW(bitmapFile2 = BitmapFile::ReadIndexed(writer.GetReader()));
	EXPECT_EQ(bitmapFile, bitmapFile2);
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
		WriteAndReadBitmapSub(1, 1, 1);
		WriteAndReadBitmapSub(1, 1, 2);
		WriteAndReadBitmapSub(1, 32, 1);
		WriteAndReadBitmapSub(1, 33, 1);
	}
	{
		SCOPED_TRACE("4 Bit");
		WriteAndReadBitmapSub(4, 1, 1);
		WriteAndReadBitmapSub(4, 1, 2);
		WriteAndReadBitmapSub(4, 8, 1);
		WriteAndReadBitmapSub(4, 9, 1);
	}
	{
		SCOPED_TRACE("8 Bit");
		WriteAndReadBitmapSub(8, 1, 1);
		WriteAndReadBitmapSub(8, 1, 2);
		WriteAndReadBitmapSub(8, 2, 1);
	}
}

TEST(BitmapFile, VerifyIndexedPaletteSizeDoesNotExceedBitCount)
{
	EXPECT_NO_THROW(BitmapFile::VerifyIndexedPaletteSizeDoesNotExceedBitCount(1, 1));
	EXPECT_NO_THROW(BitmapFile::VerifyIndexedPaletteSizeDoesNotExceedBitCount(1, 2));
	EXPECT_THROW(BitmapFile::VerifyIndexedPaletteSizeDoesNotExceedBitCount(1, 3), std::runtime_error);

	// Test non-static version of function
	BitmapFile bitmapFile = BitmapFile::CreateIndexed(1, 1, 1);
	EXPECT_NO_THROW(bitmapFile.VerifyIndexedPaletteSizeDoesNotExceedBitCount());
	bitmapFile.palette.resize(3);
	EXPECT_THROW(bitmapFile.VerifyIndexedPaletteSizeDoesNotExceedBitCount(), std::runtime_error);
}

TEST(BitmapFile, VerifyPixelSizeMatchesImageDimensionsWithPitch)
{
	EXPECT_NO_THROW(BitmapFile::VerifyPixelSizeMatchesImageDimensionsWithPitch(1, 1, 1, 4));
	EXPECT_THROW(BitmapFile::VerifyPixelSizeMatchesImageDimensionsWithPitch(1, 1, 1, 1), std::runtime_error);

	// Test non-static version of function
	BitmapFile bitmapFile = BitmapFile::CreateIndexed(1, 1, 1);;
	EXPECT_NO_THROW(bitmapFile.VerifyPixelSizeMatchesImageDimensionsWithPitch());
	bitmapFile.pixels.resize(1, 0);
	EXPECT_THROW(bitmapFile.VerifyPixelSizeMatchesImageDimensionsWithPitch(), std::runtime_error);
}

TEST(BitmapFile, CreateWithPalette)
{
	auto bitmapFile = BitmapFile::CreateIndexed(8, 2, 2, { DiscreteColor::Green });

	ASSERT_EQ(DiscreteColor::Green, bitmapFile.palette[0]);

	// Ensure all pixels are set to palette index 0 (so they register as the initial color)
	for (const auto& pixel : bitmapFile.pixels) {
		ASSERT_EQ(0u, pixel);
	}

	// Proivde palette with more indices than bit count supports
	std::vector<Color> palette{ DiscreteColor::Green, DiscreteColor::Red, DiscreteColor::Blue };
	EXPECT_THROW(bitmapFile = BitmapFile::CreateIndexed(1, 2, 2, palette), std::runtime_error);
}

TEST(BitmapFile, ScanLineOrientation)
{
	{ // Test Negative Height
		auto bitmap = BitmapFile::CreateIndexed(1, 32, -32);
		EXPECT_EQ(ScanLineOrientation::TopDown, bitmap.ScanLineOrientation());
	}
	{ // Test Positive Height
		auto bitmap = BitmapFile::CreateIndexed(1, 32, 32);
		EXPECT_EQ(ScanLineOrientation::BottomUp, bitmap.ScanLineOrientation());
	}
}

TEST(BitmapFile, AbsoluteHeight)
{
	{ // Test Positive Height
		auto bitmap = BitmapFile::CreateIndexed(1, 32, 32);
		EXPECT_EQ(32, bitmap.AbsoluteHeight());
	}
	{ // Test Negative Height
		auto bitmap = BitmapFile::CreateIndexed(1, 32, -32);
		EXPECT_EQ(32, bitmap.AbsoluteHeight());
	}
}

TEST(BitmapFile, SwapRedAndBlue)
{
	auto bitmapFile = BitmapFile::CreateIndexed(8, 2, 2, { DiscreteColor::Red });

	bitmapFile.SwapRedAndBlue();
	EXPECT_EQ(DiscreteColor::Blue, bitmapFile.palette[0]);
}

TEST(BitmapFile, InvertScanLines)
{
	const std::vector<uint8_t> pixels{
		1, 1, 1, 1, 1, 1, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0
	};

	const std::vector<uint8_t> invertedPixels{
		0, 0, 0, 0, 0, 0, 0, 0,
		1, 1, 1, 1, 1, 1, 1, 1
	};

	auto bitmap = BitmapFile::CreateIndexed(8, 8, 2, { /* Empty Palette */ }, pixels);
	bitmap.InvertScanLines();

	EXPECT_EQ(ScanLineOrientation::TopDown, bitmap.ScanLineOrientation());
	EXPECT_EQ(invertedPixels, bitmap.pixels);

	bitmap.InvertScanLines();

	EXPECT_EQ(ScanLineOrientation::BottomUp, bitmap.ScanLineOrientation());
	EXPECT_EQ(pixels, bitmap.pixels);
}

TEST(BitmapFile, Equality)
{
	BitmapFile bitmapFile1 = BitmapFile::CreateIndexed(1, 1, 1);	
	auto bitmapFile2 = bitmapFile1;

	EXPECT_TRUE(bitmapFile1 == bitmapFile2);
	EXPECT_FALSE(bitmapFile1 != bitmapFile2);

	bitmapFile1.pixels[0] = 1;
	EXPECT_FALSE(bitmapFile1 == bitmapFile2);
	EXPECT_TRUE(bitmapFile1 != bitmapFile2);
}
