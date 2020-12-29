#include "../../src/Sprite/TilesetLoader.h"
#include "../../src/Stream/MemoryReader.h"
#include "../../src/Tag.h"
#include <gtest/gtest.h>

TEST(TilesetLoader, PeekIsCustomTileset)
{
	// Ensure bool return is correct and that peek does not affect stream position

	Stream::MemoryReader reader1(&TilesetLoader::TagFileSignature, sizeof(TilesetLoader::TagFileSignature));
	EXPECT_TRUE(TilesetLoader::PeekIsCustomTileset(reader1));
	EXPECT_EQ(0u, reader1.Position());
	
	const Tag wrongFileSignature("TEST");
	Stream::MemoryReader reader2(&wrongFileSignature, sizeof(wrongFileSignature));
	EXPECT_FALSE(TilesetLoader::PeekIsCustomTileset(reader2));
	EXPECT_EQ(0u, reader2.Position());
}
