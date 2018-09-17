#include "../src/Sprites/Color.h"
#include "../src/Sprites/IndexedBmpWriter.h"
#include "XFile.h"
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdint>

TEST(IndexedBmpWriter, WriteMonochrome) {
	std::vector<Color> palette{ DiscreteColor::Red, DiscreteColor::Blue };
	std::string filename = "Sprites/data/MonochromeTest.bmp";

	std::vector<uint8_t> pixels { 255, 255, 255, 0, 0, 0 };
	EXPECT_NO_THROW(IndexedBmpWriter::Write(filename, 1, 24, -2, palette, pixels));
	EXPECT_NO_THROW(IndexedBmpWriter::Write(filename, 1, 24, 2, palette, pixels));

	pixels = std::vector<uint8_t>{ 255, 255, 255, 255, 0, 0, 0, 0 };
	EXPECT_NO_THROW(IndexedBmpWriter::Write(filename, 1, 32, -2, palette, pixels));

	pixels = std::vector<uint8_t>{ 255, 255, 255, 255, 255, 0, 0, 0, 0, 0 };
	EXPECT_NO_THROW(IndexedBmpWriter::Write(filename, 1, 40, -2, palette, pixels));

	XFile::DeletePath(filename);
}

TEST(IndexedBmpWriter, Write8Bit)
{
	std::string filename = "Sprites/data/8BitTest.bmp";

	std::vector<uint8_t> pixels{ 0, 0, 0, 1, 1, 1 };
	std::vector<Color> palette;
	palette.resize(256);
	std::fill(palette.begin(), palette.end(), DiscreteColor::Blue);
	palette[0] = DiscreteColor::Red;
	
	EXPECT_NO_THROW(IndexedBmpWriter::Write(filename, 8, 3, -2, palette, pixels));
	EXPECT_NO_THROW(IndexedBmpWriter::Write(filename, 8, 3, 2, palette, pixels));

	XFile::DeletePath(filename);
}
