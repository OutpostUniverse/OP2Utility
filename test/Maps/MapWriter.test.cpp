#include "Maps/MapWriter.h"
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
	EXPECT_NO_THROW(MapWriter::Write(testFilename, MapData()));
	XFile::DeletePath(testFilename);

	// Write to Memory
	std::vector<char> buffer(1024);
	Stream::MemoryWriter memoryWriter(buffer.data(), buffer.size() * sizeof(decltype(buffer)::value_type));
	EXPECT_NO_THROW(MapWriter::Write(memoryWriter, MapData()));
}

TEST(MapWriter, BlankFilename)
{
	EXPECT_THROW(MapWriter::Write("", MapData()), std::runtime_error);
}

TEST(MapWriter, VersionTag) {
	const std::string testFilename("Maps/data/test.map");
	MapData mapData;
	
	// The following 2 tests will require a legitimately constructed Map before running.
	//mapData.header.versionTag = minVersionTag;
	//EXPECT_NO_THROW(MapWriter::Write(testFilename, mapData));

	//mapData.header.versionTag = minVersionTag + 1;
	//EXPECT_NO_THROW(MapWriter::Write(testFilename, mapData));

	mapData.header.versionTag = minMapVersion - 1;
	EXPECT_THROW(MapWriter::Write(testFilename, mapData), std::runtime_error);

	XFile::DeletePath(testFilename);
}
