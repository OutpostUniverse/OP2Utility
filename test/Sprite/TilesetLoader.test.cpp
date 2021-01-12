#include "../../src/Sprite/TilesetLoader.h"
#include "../../src/Bitmap/BitmapFile.h"
#include "../../src/Stream/MemoryReader.h"
#include "../../src/Stream/DynamicMemoryWriter.h"
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

TEST(TilesetLoader, WriteCustomTileset)
{
	Stream::DynamicMemoryWriter writer;

	auto tileset1 = BitmapFile::CreateIndexed(8, 32, 32, { DiscreteColor::Red});
	Tileset::WriteCustomTileset(writer, tileset1);

	// Read just written tileset to ensure it was well formed
	auto tileset2 = Tileset::ReadTileset(writer.GetReader());

	EXPECT_EQ(tileset1, tileset2);
}

TEST(TilesetLoader, WriteCustomTilesetError)
{
	Stream::DynamicMemoryWriter writer;

	// Use incorrect pixel width - Ensure error is thrown
	auto tileset = BitmapFile::CreateIndexed(8, 20, 32, { DiscreteColor::Red });
	EXPECT_THROW(Tileset::WriteCustomTileset(writer, tileset), std::runtime_error);
}

TEST(TilesetLoader, ReadTileset)
{	
	// Well formed standard bitmap
	auto tileset = BitmapFile::CreateIndexed(8, 32, 32, { DiscreteColor::Red });
	Stream::DynamicMemoryWriter writer;
	tileset.WriteIndexed(writer);
	
	auto newTileset = Tileset::ReadTileset(writer.GetReader());

	EXPECT_EQ(tileset, newTileset);

	// Well formed standard bitmap - Wrong width for a tileset
	tileset = BitmapFile::CreateIndexed(8, 20, 32, { DiscreteColor::Red });
	tileset.WriteIndexed(writer);
	EXPECT_THROW(Tileset::ReadTileset(writer.GetReader()), std::runtime_error);
}

TEST(TilesetLoader, ValidateTileset)
{
	EXPECT_NO_THROW(Tileset::ValidateTileset(BitmapFile::CreateIndexed(8, 32, 32)));

	// Improper bit depth
	EXPECT_THROW(Tileset::ValidateTileset(BitmapFile::CreateIndexed(1, 32, 32)), std::runtime_error);

	// Improper width
	EXPECT_THROW(Tileset::ValidateTileset(BitmapFile::CreateIndexed(1, 64, 32)), std::runtime_error);

	// Improper Height
	EXPECT_THROW(Tileset::ValidateTileset(BitmapFile::CreateIndexed(1, 32, 70)), std::runtime_error);
}
