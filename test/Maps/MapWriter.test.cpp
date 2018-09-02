#include "Maps/MapWriter.h"
#include "Streams/MemoryWriter.h"
#include "XFile.h"
#include <gtest/gtest.h>
#include <vector>
#include <stdexcept>

TEST(MapWriter, EmptyMapData)
{
	MapData mapData;
	
	// Write to File
	const std::string testFilename("maps/data/test.map");
	EXPECT_THROW(MapWriter::Write(testFilename, mapData), std::runtime_error);
	XFile::DeletePath(testFilename);

	// Write to Memory
	std::vector<char> buffer;
	buffer.resize(1000);
	Stream::MemoryWriter memoryWriter(buffer.data(), buffer.size() * sizeof(decltype(buffer)::value_type));
	EXPECT_THROW(MapWriter::Write(memoryWriter, mapData), std::runtime_error);
}

TEST(MapWriter, BlankFilename)
{
	MapData mapData;

	EXPECT_THROW(MapWriter::Write("", mapData), std::runtime_error);
}
