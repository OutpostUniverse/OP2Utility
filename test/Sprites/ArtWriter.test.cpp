#include "../src/Sprites/ArtFile.h"
#include "../src/XFile.h"
#include "Streams/MemoryWriter.h"
#include <gtest/gtest.h>
#include <string>

TEST(ArtWriter, Empty)
{
	// Write to File
	const std::string testFilename("Sprites/data/test.prt");
	EXPECT_NO_THROW(ArtFile::Write(testFilename, ArtFile()));
	XFile::DeletePath(testFilename);

	// Write to Memory
	std::vector<char> buffer(1024);
	Stream::MemoryWriter memoryWriter(buffer.data(), buffer.size() * sizeof(decltype(buffer)::value_type));
	EXPECT_NO_THROW(ArtFile::Write(memoryWriter, ArtFile()));
}

TEST(ArtWriter, BlankFilename)
{
	EXPECT_THROW(ArtFile::Write("", ArtFile()), std::runtime_error);
}
