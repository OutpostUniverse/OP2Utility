#include "Maps/TilesetSource.h"
#include <gtest/gtest.h>

TEST(MapTilesetSource, IsEmpty) {
	ASSERT_TRUE((TilesetSource{"", 0}).IsEmpty());
	ASSERT_TRUE((TilesetSource{"", 1}).IsEmpty());
	ASSERT_TRUE((TilesetSource{"filename.bmp", 0}).IsEmpty());
	ASSERT_FALSE((TilesetSource{"well0000.bmp", 1}).IsEmpty());
}
