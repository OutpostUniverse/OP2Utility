#include "Map/Map.h"
#include <gtest/gtest.h>

TEST(Map, TrimTilesetSources) {
	Map map;

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

TEST(Map, CellType) {
	Map map;
	map.tiles.push_back(Tile());

	EXPECT_NO_THROW(map.SetCellType(CellType::FastPassible2, 0, 0));
	EXPECT_EQ(CellType::FastPassible2, map.GetCellType(0, 0));

	// Pass a non-defined cell type and receive an error
	EXPECT_THROW(map.SetCellType(static_cast<CellType>(9999), 0, 0), std::runtime_error);
	// Ensure no change in original cell type after exception raised
	EXPECT_EQ(CellType::FastPassible2, map.GetCellType(0, 0));
}

TEST(Map, LavaPossible) {
	Map map;
	map.tiles.push_back(Tile());

	EXPECT_NO_THROW(map.SetLavaPossible(true, 0, 0));
	EXPECT_TRUE(map.GetLavaPossible(0, 0));

	EXPECT_NO_THROW(map.SetLavaPossible(false, 0, 0));
	EXPECT_FALSE(map.GetLavaPossible(0, 0));
}
