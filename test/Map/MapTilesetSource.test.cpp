#include "Map/TilesetSource.h"
#include <gtest/gtest.h>

using namespace OP2Utility;

TEST(MapTilesetSource, ComparisonOperators) {
	const TilesetSource blue{"well0000.bmp", 1};
	const TilesetSource well0{"well0000.bmp", 1};
	const TilesetSource well1{"well0001.bmp", 200};

	// Note: Internally EXPECT_EQ uses `operator ==`
	ASSERT_EQ(blue, well0);
	ASSERT_EQ(well0, well0);
	ASSERT_EQ(well1, well1);

	// Note: Internally EXPECT_NE uses `operator !=`
	ASSERT_NE(well0, well1);
	ASSERT_NE(well1, well0);
}

TEST(MapTilesetSource, IsEmpty) {
	ASSERT_TRUE((TilesetSource{"", 0}).IsEmpty());
	ASSERT_TRUE((TilesetSource{"", 1}).IsEmpty());
	ASSERT_TRUE((TilesetSource{"filename.bmp", 0}).IsEmpty());
	ASSERT_FALSE((TilesetSource{"well0000.bmp", 1}).IsEmpty());
}
