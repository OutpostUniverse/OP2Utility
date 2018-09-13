#include "../src/Sprites/ArtFile.h"
#include "../src/XFile.h"
#include "Streams/MemoryWriter.h"
#include "XFile.h"
#include <gtest/gtest.h>
#include <string>
#include <stdexcept>

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

TEST(ArtWriter, scanLineByteWidth)
{
	ArtFile artFile;
	artFile.palettes.push_back(Palette());

	ImageMeta imageMeta;
	imageMeta.width = 10;
	imageMeta.scanLineByteWidth = 22;
	imageMeta.paletteIndex = 0;

	artFile.imageMetas.push_back(imageMeta);

	std::string filename = "Sprites/data/test.prt";

	EXPECT_THROW(ArtFile::Write(filename, artFile), std::runtime_error);

	XFile::DeletePath(filename);
}

TEST(ArtWriter, paletteIndex) 
{
	ArtFile artFile;

	ImageMeta imageMeta;
	imageMeta.width = 10;
	imageMeta.scanLineByteWidth = 32;
	imageMeta.paletteIndex = 0;

	artFile.imageMetas.push_back(imageMeta);

	std::string filename = "Sprites/data/test.prt";

	EXPECT_THROW(ArtFile::Write(filename, artFile), std::runtime_error);

	XFile::DeletePath(filename);
}
