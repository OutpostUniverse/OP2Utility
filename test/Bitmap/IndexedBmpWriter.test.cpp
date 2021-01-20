#include "../src/Bitmap/Color.h"
#include "../src/Bitmap/BitmapFile.h"
#include "../src/Stream/DynamicMemoryWriter.h"
#include "XFile.h"
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <cstdint>

using namespace Stream;

TEST(BitmapFile, InvalidBitCountThrows)
{
	const std::vector<Color> palette(8);
	const std::vector<uint8_t> pixels(4, 0);

	EXPECT_THROW(BitmapFile::CreateIndexed(3, 1, 1, palette, pixels).WriteIndexed(DynamicMemoryWriter()), std::runtime_error);
	EXPECT_THROW(BitmapFile::CreateIndexed(32, 1, 1, palette, pixels).WriteIndexed(DynamicMemoryWriter()), std::runtime_error);
}

TEST(BitmapFile, TooManyPaletteEntriesThrows)
{
	const std::vector<Color> palette(3);
	const std::vector<uint8_t> pixels(4, 0);

	EXPECT_THROW(BitmapFile::CreateIndexed(1, 1, 1, palette, pixels).WriteIndexed(DynamicMemoryWriter()), std::runtime_error);
}

TEST(BitmapFile, WritePartiallyFilledPalette)
{
	const std::vector<Color> palette(1);
	const std::vector<uint8_t> pixels(4, 0);

	EXPECT_NO_THROW(BitmapFile::CreateIndexed(1, 1, 1, palette, pixels).WriteIndexed(DynamicMemoryWriter()));
}

TEST(BitmapFile, IncorrectPixelPaddingThrows)
{
	const std::vector<Color> palette(2);

	std::vector<uint8_t> pixels(3, 0);
	EXPECT_THROW(BitmapFile::CreateIndexed(1, 1, 1, palette, pixels).WriteIndexed(DynamicMemoryWriter()), std::runtime_error);

	pixels.resize(5, 0);
	EXPECT_THROW(BitmapFile::CreateIndexed(1, 1, 1, palette, pixels).WriteIndexed(DynamicMemoryWriter()), std::runtime_error);
}
