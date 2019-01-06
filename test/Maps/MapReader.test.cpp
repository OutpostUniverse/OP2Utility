#include "Maps/MapData.h"
#include <gtest/gtest.h>
#include <string>

TEST(MapReader, MissingFile) {
	EXPECT_THROW(MapData::ReadMap("MissingFile.map"), std::runtime_error);
	EXPECT_THROW(MapData::ReadSavedGame("MissingFile.op2"), std::runtime_error);

	// Check if filename is an empty string
	EXPECT_THROW(MapData::ReadMap(""), std::runtime_error);
	EXPECT_THROW(MapData::ReadSavedGame(""), std::runtime_error);
}

TEST(MapReader, EmptyFile) {
	EXPECT_THROW(MapData::ReadMap("Maps/data/EmptyMap.map"), std::runtime_error);
	EXPECT_THROW(MapData::ReadSavedGame("Maps/data/EmptySave.OP2"), std::runtime_error);
}
