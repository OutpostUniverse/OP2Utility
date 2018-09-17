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

	pixels = std::vector<uint8_t>{ 0, 0, 0, 1, 1, 1 };
	std::vector<Color> palette8Bit;
	palette8Bit.resize(256);
	std::fill(palette8Bit.begin(), palette8Bit.end(), DiscreteColor::Blue);
	palette8Bit[0] = DiscreteColor::Red;
	EXPECT_NO_THROW(IndexedBmpWriter::Write(filename, 8, 3, -2, palette8Bit, pixels));
	EXPECT_NO_THROW(IndexedBmpWriter::Write(filename, 8, 3, 2, palette8Bit, pixels));

	//XFile::DeletePath(filename);
}
