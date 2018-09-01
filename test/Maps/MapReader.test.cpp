#include "Maps/MapReader.h"
#include "Maps/MapType.h"
#include <gtest/gtest.h>
#include <string>

TEST(MapReader, MissingFile) {
	EXPECT_THROW(MapReader::Read("MissingFile.map", MapType::Map), std::runtime_error);
	EXPECT_THROW(MapReader::Read("MissingFile.op2", MapType::Save), std::runtime_error);

	// Check if filename is an empty string
	EXPECT_THROW(MapReader::Read(std::string(""), MapType::Map), std::runtime_error);
	EXPECT_THROW(MapReader::Read(std::string(""), MapType::Save), std::runtime_error);
}

TEST(MapReader, EmptyFile) {
	EXPECT_THROW(MapReader::Read("Maps/data/EmptyMap.map", MapType::Map), std::runtime_error);
	EXPECT_THROW(MapReader::Read("Maps/data/EmptySave.OP2", MapType::Save), std::runtime_error);
}
