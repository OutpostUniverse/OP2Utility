#include "../src/Sprites/Color.h"
#include "../src/Sprites/IndexedBmpWriter.h"
#include "XFile.h"
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <cstdint>

TEST(IndexedBmpWriter, WriteMonochrome) 
{
	std::vector<Color> palette{ DiscreteColor::Black, DiscreteColor::White };
	std::string filename = "Sprites/data/MonochromeTest.bmp";

	std::vector<uint8_t> pixels{ 128, 0, 0, 0, 128 , 0, 0, 0};
	EXPECT_NO_THROW(IndexedBmpWriter::Write(filename, 1, 1, 2, palette, pixels));

	// Test padding 1 byte less than pitch
	pixels = std::vector<uint8_t>{ 128, 0, 0, 128, 0, 0 };
	EXPECT_THROW(IndexedBmpWriter::Write(filename, 1, 1, 2, palette, pixels), std::runtime_error);

	// Test padding 1 byte greater than pitch
	pixels = std::vector<uint8_t>{ 128, 0, 0, 0, 0, 128 , 0, 0, 0, 0 };
	EXPECT_THROW(IndexedBmpWriter::Write(filename, 1, 1, 2, palette, pixels), std::runtime_error);

	XFile::DeletePath(filename);
}

TEST(IndexedBmpWriter, Write4Bit)
{
	std::string filename = "Sprites/data/4BitTest.bmp";
	std::vector<uint8_t> pixels{ 0, 0, 0, 0, 0, 0, 0, 0 };
	std::vector<Color> palette(16, DiscreteColor::Black);
	palette[0] = DiscreteColor::White;

	EXPECT_NO_THROW(IndexedBmpWriter::Write(filename, 4, 1, 2, palette, pixels));

	// Test padding 1 byte less than pitch
	pixels = std::vector<uint8_t>{ 0, 0, 0, 0, 0, 0 };
	EXPECT_THROW(IndexedBmpWriter::Write(filename, 8, 1, 2, palette, pixels), std::runtime_error);

	// Test padding 1 byte greater than pitch
	pixels = std::vector<uint8_t>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	EXPECT_THROW(IndexedBmpWriter::Write(filename, 8, 1, 2, palette, pixels), std::runtime_error);

	XFile::DeletePath(filename);
}

TEST(IndexedBmpWriter, Write8Bit)
{
	std::string filename = "Sprites/data/8BitTest.bmp";

	std::vector<uint8_t> pixels{ 0, 0, 0, 0, 0, 0, 0, 0 };
	std::vector<Color> palette(256, DiscreteColor::Black);
	palette[0] = DiscreteColor::White;
	
	EXPECT_NO_THROW(IndexedBmpWriter::Write(filename, 8, 1, 2, palette, pixels));

	// Test padding 1 byte less than pitch
	pixels = std::vector<uint8_t>{ 0, 0, 0, 0, 0, 0 };
	EXPECT_THROW(IndexedBmpWriter::Write(filename, 8, 1, 2, palette, pixels), std::runtime_error);

	// Test padding 1 byte greater than pitch
	pixels = std::vector<uint8_t>{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	EXPECT_THROW(IndexedBmpWriter::Write(filename, 8, 1, 2, palette, pixels), std::runtime_error);

	XFile::DeletePath(filename);
}
