#include "../../src/Sprite/TilesetLoader.h"
#include "../../src/Bitmap/BitmapFile.h"
#include "../../src/Stream/MemoryReader.h"
#include "../../src/Tag.h"
#include <gtest/gtest.h>
#include <stdexcept>

TEST(TilesetLoader, PeekIsCustomTileset)
{
	// Ensure bool return is correct and that peek does not affect stream position

	Stream::MemoryReader reader1(&Tileset::TagFileSignature, sizeof(Tileset::TagFileSignature));
	EXPECT_TRUE(Tileset::PeekIsCustomTileset(reader1));
	EXPECT_EQ(0u, reader1.Position());
	
	const Tag wrongFileSignature("TEST");
	Stream::MemoryReader reader2(&wrongFileSignature, sizeof(wrongFileSignature));
	EXPECT_FALSE(Tileset::PeekIsCustomTileset(reader2));
	EXPECT_EQ(0u, reader2.Position());
}

TEST(TilesetLoader, ValidateTileset)
{
	EXPECT_NO_THROW(Tileset::ValidateTileset(BitmapFile::CreateDefaultIndexed(8, 32, 32)));

	// Improper bit depth
	EXPECT_THROW(Tileset::ValidateTileset(BitmapFile::CreateDefaultIndexed(1, 32, 32)), std::runtime_error);

	// Improper width
	EXPECT_THROW(Tileset::ValidateTileset(BitmapFile::CreateDefaultIndexed(1, 64, 32)), std::runtime_error);

	// Improper Height
	EXPECT_THROW(Tileset::ValidateTileset(BitmapFile::CreateDefaultIndexed(1, 32, 70)), std::runtime_error);
}
