#include "Maps/MapData.h"
#include "Maps/MapHeader.h"
#include "Streams/MemoryWriter.h"
#include "XFile.h"
#include <gtest/gtest.h>
#include <vector>
#include <stdexcept>

TEST(MapWriter, EmptyMapData)
{
	// Write to File
	const std::string testFilename("Maps/data/test.map");
	EXPECT_NO_THROW(MapData::Write(testFilename, MapData()));
	XFile::DeletePath(testFilename);

	// Write to Memory
	std::vector<char> buffer(1024);
	Stream::MemoryWriter memoryWriter(buffer.data(), buffer.size() * sizeof(decltype(buffer)::value_type));
	EXPECT_NO_THROW(MapData::Write(memoryWriter, MapData()));
}

TEST(MapWriter, BlankFilename)
{
	EXPECT_THROW(MapData::Write("", MapData()), std::runtime_error);
}

TEST(MapWriter, AllowInvalidVersionTag) {
	const std::string testFilename("Maps/data/test.map");
	MapData mapData;

	mapData.SetVersionTag(minMapVersion - 1);
	EXPECT_NO_THROW(MapData::Write(testFilename, mapData));

	XFile::DeletePath(testFilename);
}
