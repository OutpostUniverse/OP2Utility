#include "Map/MapHeader.h"
#include <gtest/gtest.h>

TEST(MapHeader, VersionTagValid) {
	MapHeader mapHeader;

	EXPECT_TRUE(mapHeader.VersionTagValid());

	mapHeader.versionTag = MapHeader::MinMapVersion + 1;
	EXPECT_TRUE(mapHeader.VersionTagValid());

	mapHeader.versionTag = MapHeader::MinMapVersion - 1;
	EXPECT_FALSE(mapHeader.VersionTagValid());
}

class MapHeader32TileSquare : public ::testing::Test {
public:
	MapHeader32TileSquare() : mapHeader(MapHeader()) {
		mapHeader.lgWidthInTiles = 5;
		mapHeader.heightInTiles = 32;
	}
protected:
	MapHeader mapHeader;
};

TEST_F(MapHeader32TileSquare, WidthInTiles) {
	EXPECT_EQ(32, mapHeader.WidthInTiles());
}

TEST_F(MapHeader32TileSquare, TileCount) {
	EXPECT_EQ(32 * 32, mapHeader.TileCount());
}
