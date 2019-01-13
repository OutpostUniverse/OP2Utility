#include "Maps/Map.h"
#include "Maps/MapHeader.h"
#include "Streams/MemoryWriter.h"
#include "XFile.h"
#include <gtest/gtest.h>
#include <vector>
#include <stdexcept>

TEST(MapWriter, EmptyMap)
{
	// Write to File
	const std::string testFilename("Maps/data/test.map");
	EXPECT_NO_THROW(Map().Write(testFilename));
	XFile::DeletePath(testFilename);

	// Write to Memory
	std::vector<char> buffer(1024);
	Stream::MemoryWriter memoryWriter(buffer.data(), buffer.size() * sizeof(decltype(buffer)::value_type));
	EXPECT_NO_THROW(Map().Write(memoryWriter));
}

TEST(MapWriter, BlankFilename)
{
	EXPECT_THROW(Map().Write(""), std::runtime_error);
}

TEST(MapWriter, AllowInvalidVersionTag) {
	const std::string testFilename("Maps/data/test.map");
	Map map;

	map.SetVersionTag(MapHeader::MinMapVersion - 1);
	EXPECT_NO_THROW(Map().Write(testFilename));

	XFile::DeletePath(testFilename);
}
