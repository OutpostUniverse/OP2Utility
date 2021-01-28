#include "Map/Map.h"
#include "Map/MapHeader.h"
#include "Stream/DynamicMemoryWriter.h"
#include "XFile.h"
#include <gtest/gtest.h>
#include <vector>
#include <stdexcept>

using namespace OP2Utility;

TEST(MapWriter, EmptyMap)
{
	// Write to File
	const std::string testFilename("Map/data/test.map");
	EXPECT_NO_THROW(Map().Write(testFilename));
	XFile::DeletePath(testFilename);

	// Write to Memory
	Stream::DynamicMemoryWriter writer;
	EXPECT_NO_THROW(Map().Write(writer));
}

TEST(MapWriter, BlankFilename)
{
	EXPECT_THROW(Map().Write(""), std::runtime_error);
}

TEST(MapWriter, AllowInvalidVersionTag) {
	Stream::DynamicMemoryWriter writer;
	Map map;

	map.SetVersionTag(MapHeader::MinMapVersion - 1);
	EXPECT_NO_THROW(Map().Write(writer));
}
