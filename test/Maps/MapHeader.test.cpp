#include "Maps/MapHeader.h"
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
		mapHeader.lgMapTileWidth = 5;
		mapHeader.mapTileHeight = 32;
	}
protected:
	MapHeader mapHeader;
};

TEST_F(MapHeader32TileSquare, MapTileWidth) {
	EXPECT_EQ(32, mapHeader.MapTileWidth());
}

TEST_F(MapHeader32TileSquare, TileCount) {
	EXPECT_EQ(32 * 32, mapHeader.TileCount());
}
