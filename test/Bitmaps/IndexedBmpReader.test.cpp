#include "../src/Bitmaps/BitmapFile.h"
#include <gtest/gtest.h>

TEST(IndexedBmpReader, Read)
{
	BitmapFile bitmapFile;
	EXPECT_NO_THROW(bitmapFile = BitmapFile::ReadIndexed("Bitmaps/data/1x2Monochrome.bmp"));

	EXPECT_EQ(1, bitmapFile.imageHeader.bitCount);
	EXPECT_EQ(1, bitmapFile.imageHeader.width);
	EXPECT_EQ(2, bitmapFile.imageHeader.height);
}
