#include "../src/Sprites/Color.h"
#include "../src/Sprites/IndexedBmpWriter.h"
#include "XFile.h"
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <cstdint>

TEST(IndexedBmpWriter, WriteMonochrome) 
{
	std::vector<Color> palette{ DiscreteColor::Red, DiscreteColor::Blue };
	std::string filename = "Sprites/data/MonochromeTest.bmp";

	std::vector<uint8_t> pixels { 255, 255, 255, 0, 0, 0 };

	// Test width 1 byte less than scan line
	EXPECT_NO_THROW(IndexedBmpWriter::Write(filename, 1, 24, -2, palette, pixels));
	EXPECT_NO_THROW(IndexedBmpWriter::Write(filename, 1, 24, 2, palette, pixels));

	// Test width equal to scan line
	pixels = std::vector<uint8_t>{ 255, 255, 255, 255, 0, 0, 0, 0 };
	EXPECT_NO_THROW(IndexedBmpWriter::Write(filename, 1, 32, -2, palette, pixels));

	// Test width 1 byte greater than scan line
	pixels = std::vector<uint8_t>{ 255, 255, 255, 255, 255, 0, 0, 0, 0, 0 };
	EXPECT_NO_THROW(IndexedBmpWriter::Write(filename, 1, 40, -2, palette, pixels));

	XFile::DeletePath(filename);
}

TEST(IndexedBmpWriter, Write4Bit)
{
	std::string filename = "Sprites/data/4BitTest.bmp";
	std::vector<uint8_t> pixels{ 0, 0, 0b00010001,  0b00010001 };
	std::vector<Color> palette(16, DiscreteColor::Blue);
	palette[0] = DiscreteColor::Red;

	EXPECT_NO_THROW(IndexedBmpWriter::Write(filename, 4, 4, -2, palette, pixels));
	EXPECT_NO_THROW(IndexedBmpWriter::Write(filename, 4, 4, 2, palette, pixels));

	XFile::DeletePath(filename);
}

TEST(IndexedBmpWriter, Write8Bit)
{
	std::string filename = "Sprites/data/8BitTest.bmp";

	std::vector<uint8_t> pixels{ 0, 0, 0, 1, 1, 1 };
	std::vector<Color> palette(256, DiscreteColor::Blue);
	palette[0] = DiscreteColor::Red;
	
	EXPECT_NO_THROW(IndexedBmpWriter::Write(filename, 8, 3, -2, palette, pixels));
	EXPECT_NO_THROW(IndexedBmpWriter::Write(filename, 8, 3, 2, palette, pixels));

	XFile::DeletePath(filename);
}
