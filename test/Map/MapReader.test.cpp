#include "Map/Map.h"
#include <gtest/gtest.h>
#include <string>

TEST(MapReader, MissingFile) {
	EXPECT_THROW(Map::ReadMap("MissingFile.map"), std::runtime_error);
	EXPECT_THROW(Map::ReadSavedGame("MissingFile.op2"), std::runtime_error);

	// Check if filename is an empty string
	EXPECT_THROW(Map::ReadMap(""), std::runtime_error);
	EXPECT_THROW(Map::ReadSavedGame(""), std::runtime_error);
}

TEST(MapReader, EmptyFile) {
	EXPECT_THROW(Map::ReadMap("Map/data/EmptyMap.map"), std::runtime_error);
	EXPECT_THROW(Map::ReadSavedGame("Map/data/EmptySave.OP2"), std::runtime_error);
}
