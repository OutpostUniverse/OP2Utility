#include "Map/Map.h"
#include "Stream/DynamicMemoryWriter.h"
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

TEST(MapReader, ReadMap) {
	// Simple valid source to load from
	Stream::DynamicMemoryWriter writer;
	Map().Write(writer);

	// Read from stream as lvalue
	auto reader = writer.GetReader();
	EXPECT_NO_THROW(auto mapFile = Map::ReadMap(reader));

	// Read from stream as rvalue (unnamed temporary object)
	EXPECT_NO_THROW(auto mapFile = Map::ReadMap(writer.GetReader()));
	
	// Throw error if attempting to read a saved game from a map
	EXPECT_THROW(auto savedGame = Map::ReadSavedGame(writer.GetReader()), std::runtime_error);
}
