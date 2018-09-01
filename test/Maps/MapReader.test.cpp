#include "Maps/MapReader.h"
#include "gtest/gtest.h"
#include <string>

TEST(MapReader, EmptyFilename) {
	EXPECT_THROW(MapReader::Read(std::string(), false), std::runtime_error);
	EXPECT_THROW(MapReader::Read(std::string(), true), std::runtime_error);
}

TEST(MapReader, MissingFile) {
	EXPECT_THROW(MapReader::Read("MissingFile.map", false), std::runtime_error);
	EXPECT_THROW(MapReader::Read("MissingFile.op2", true), std::runtime_error);
}

//TEST(MapReader, EmptyFile) {
//	EXPECT_THROW(MapReader::Read("Maps/data/EmptyMap.map", false), std::runtime_error);
//	EXPECT_THROW(MapReader::Read("Maps/data/EmptySave.OP2", true), std::runtime_error);
//}

//TEST(MapReader, WrongExtension) {
//	EXPECT_THROW(MapReader::Read("Maps/data/WrongExtension.txt", false), std::runtime_error);
//	EXPECT_THROW(MapReader::Read("Maps/data/WrongExtension.txt", true), std::runtime_error);
//}
