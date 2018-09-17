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

class SimpleArtFile : public ::testing::Test {
public:
	inline SimpleArtFile() { 
		artFile.palettes.push_back(Palette());

		ImageMeta imageMeta;
		imageMeta.width = 10;
		imageMeta.scanLineByteWidth = 12;
		imageMeta.paletteIndex = 0;
		artFile.imageMetas.push_back(imageMeta);
	}
protected:
	ArtFile artFile;
};

TEST_F(SimpleArtFile, Write_ScanLineByteWidth)
{
	std::string filename = "Sprites/data/test.prt";

	// Check no throw if scanLine next 4 byte aligned
	EXPECT_NO_THROW(ArtFile::Write(filename, artFile));

	// Check throw if scanLine > width && < 4 byte aligned
	artFile.imageMetas[0].scanLineByteWidth = 11;
	EXPECT_THROW(ArtFile::Write(filename, artFile), std::runtime_error);

	// Check throw if scanLine > first 4 byte align
	artFile.imageMetas[0].scanLineByteWidth = 16;
	EXPECT_THROW(ArtFile::Write(filename, artFile), std::runtime_error);

	// Check throw if scanLine < width but still 4 byte aligned
	artFile.imageMetas[0].scanLineByteWidth = 8;
	EXPECT_THROW(ArtFile::Write(filename, artFile), std::runtime_error);

	XFile::DeletePath(filename);
}

TEST_F(SimpleArtFile, Write_PaletteIndexRange) 
{
	std::string filename = "Sprites/data/test.prt";

	// Check for no throw when ImageMeta.paletteIndex is within palette container's range
	EXPECT_NO_THROW(ArtFile::Write(filename, artFile));

	artFile.palettes.clear();

	// Check for throw due to ImageMeta.paletteIndex outside of palette container's range
	EXPECT_THROW(ArtFile::Write(filename, artFile), std::runtime_error);

	XFile::DeletePath(filename);
}

TEST_F(SimpleArtFile, Write_PaletteColors)
{
	const uint8_t red = 255;
	const uint8_t blue = 0;
	artFile.palettes[0][0] = Color{ red, 0, blue, 0 };

	std::string filename = "Sprites/data/test.prt";

	EXPECT_NO_THROW(ArtFile::Write(filename, artFile));

	// Check ArtFile palette remains unchanged after write
	EXPECT_EQ(red, artFile.palettes[0][0].red);
	EXPECT_EQ(blue, artFile.palettes[0][0].blue);

	// Check ArtFile palette written to disk properly
	artFile = ArtFile::Read(filename);
	EXPECT_EQ(red, artFile.palettes[0][0].red);
	EXPECT_EQ(blue, artFile.palettes[0][0].blue);

	XFile::DeletePath(filename);
}
