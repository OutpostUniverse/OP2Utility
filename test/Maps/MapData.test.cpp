#include "Maps/MapData.h"
#include <gtest/gtest.h>

TEST(MapData, TrimTilesetSources) {
	MapData map;

	map.tilesetSources = {
		{"well0000.bmp", 1},
		{"somename.bmp", 0},
		{"", 1},
		{"well0001.bmp", 200}
	};
	map.TrimTilesetSources();

	ASSERT_EQ(
		(std::vector<TilesetSource>{
			{"well0000.bmp", 1},
			{"well0001.bmp", 200}
		}),
		map.tilesetSources
	);
}
