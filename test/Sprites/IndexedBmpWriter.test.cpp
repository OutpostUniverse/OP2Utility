#include "../src/Sprites/Color.h"
#include "../src/Sprites/IndexedBmpWriter.h"
#include "XFile.h"
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <cstdint>

TEST(IndexedBmpWriter, WriteAllPalettizedBitDepths)
{
	const std::string filename = "Sprites/data/BitmapTest.bmp";
	const std::vector<uint8_t> pixels(4, 0);

	std::vector<Color> palette(2);
	EXPECT_NO_THROW(IndexedBmpWriter::Write(filename, 1, 1, 1, palette, pixels));

	palette.resize(16);
	EXPECT_NO_THROW(IndexedBmpWriter::Write(filename, 4, 1, 1, palette, pixels));

	palette.resize(256);
	EXPECT_NO_THROW(IndexedBmpWriter::Write(filename, 8, 1, 1, palette, pixels));

	XFile::DeletePath(filename);
}

TEST(IndexedBmpWriter, InvalidBitCountThrows)
{
	const std::vector<Color> palette(8);
	const std::vector<uint8_t> pixels(4, 0);
	const std::string filename = "Sprites/data/MonochromeTest.bmp";

	EXPECT_THROW(IndexedBmpWriter::Write(filename, 3, 1, 1, palette, pixels), std::runtime_error);
	EXPECT_THROW(IndexedBmpWriter::Write(filename, 32, 1, 1, palette, pixels), std::runtime_error);

	XFile::DeletePath(filename);
}

TEST(IndexedBmpWriter, TooManyPaletteEntriesThrows)
{
	const std::vector<Color> palette(3);
	const std::vector<uint8_t> pixels(4, 0);
	const std::string filename("Sprites/data/PaletteRangeTest.bmp");

	EXPECT_THROW(IndexedBmpWriter::Write(filename, 1, 1, 1, palette, pixels), std::runtime_error);

	XFile::DeletePath(filename);
}

TEST(IndexedBmpWriter, WritePartiallyFilledPalette)
{
	const std::vector<Color> palette(1);
	const std::vector<uint8_t> pixels(4, 0);
	const std::string filename("Sprites/data/PaletteRangeTest.bmp");

	EXPECT_NO_THROW(IndexedBmpWriter::Write(filename, 1, 1, 1, palette, pixels));

	XFile::DeletePath(filename);
}

TEST(IndexedBmpWriter, IncorrectPixelPaddingThrows)
{
	const std::vector<Color> palette(2);
	const std::string filename("Sprites/data/IncorrectPixelPaddingTest.bmp");

	std::vector<uint8_t> pixels(3, 0);
	EXPECT_THROW(IndexedBmpWriter::Write(filename, 1, 1, 1, palette, pixels), std::runtime_error);

	pixels.resize(5, 0);
	EXPECT_THROW(IndexedBmpWriter::Write(filename, 1, 1, 1, palette, pixels), std::runtime_error);

	XFile::DeletePath(filename);
}
