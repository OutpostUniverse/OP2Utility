#include "Maps/MapReader.h"
#include <gtest/gtest.h>
#include <string>

TEST(MapReader, MissingFile) {
	EXPECT_THROW(MapReader::ReadMap("MissingFile.map"), std::runtime_error);
	EXPECT_THROW(MapReader::ReadSavedGame("MissingFile.op2"), std::runtime_error);

	// Check if filename is an empty string
	EXPECT_THROW(MapReader::ReadMap(""), std::runtime_error);
	EXPECT_THROW(MapReader::ReadSavedGame(""), std::runtime_error);
}

TEST(MapReader, EmptyFile) {
	EXPECT_THROW(MapReader::ReadMap("Maps/data/EmptyMap.map"), std::runtime_error);
	EXPECT_THROW(MapReader::ReadSavedGame("Maps/data/EmptySave.OP2"), std::runtime_error);
}
