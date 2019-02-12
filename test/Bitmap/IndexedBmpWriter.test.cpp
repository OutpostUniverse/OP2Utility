#include "../src/Bitmap/Color.h"
#include "../src/Bitmap/BitmapFile.h"
#include "XFile.h"
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <cstdint>

TEST(BitmapFile, InvalidBitCountThrows)
{
	const std::vector<Color> palette(8);
	const std::vector<uint8_t> pixels(4, 0);
	const std::string filename = "Sprite/data/MonochromeTest.bmp";

	EXPECT_THROW(BitmapFile::WriteIndexed(filename, 3, 1, 1, 4, palette, pixels), std::runtime_error);
	EXPECT_THROW(BitmapFile::WriteIndexed(filename, 32, 1, 1, 4, palette, pixels), std::runtime_error);

	XFile::DeletePath(filename);
}

TEST(BitmapFile, TooManyPaletteEntriesThrows)
{
	const std::vector<Color> palette(3);
	const std::vector<uint8_t> pixels(4, 0);
	const std::string filename("Sprite/data/PaletteRangeTest.bmp");

	EXPECT_THROW(BitmapFile::WriteIndexed(filename, 1, 1, 1, 4, palette, pixels), std::runtime_error);

	XFile::DeletePath(filename);
}

TEST(BitmapFile, WritePartiallyFilledPalette)
{
	const std::vector<Color> palette(1);
	const std::vector<uint8_t> pixels(4, 0);
	const std::string filename("Sprite/data/PaletteRangeTest.bmp");

	EXPECT_NO_THROW(BitmapFile::WriteIndexed(filename, 1, 1, 1, 4, palette, pixels));

	XFile::DeletePath(filename);
}

TEST(BitmapFile, IncorrectPixelPaddingThrows)
{
	const std::vector<Color> palette(2);
	const std::string filename("Sprite/data/IncorrectPixelPaddingTest.bmp");

	std::vector<uint8_t> pixels(3, 0);
	EXPECT_THROW(BitmapFile::WriteIndexed(filename, 1, 1, 1, 4, palette, pixels), std::runtime_error);

	pixels.resize(5, 0);
	EXPECT_THROW(BitmapFile::WriteIndexed(filename, 1, 1, 1, 4, palette, pixels), std::runtime_error);

	XFile::DeletePath(filename);
}
