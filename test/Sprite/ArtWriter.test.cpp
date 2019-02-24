#include "../src/Sprite/ArtFile.h"
#include "../src/XFile.h"
#include "Stream/DynamicMemoryWriter.h"
#include "XFile.h"
#include <gtest/gtest.h>
#include <string>
#include <stdexcept>

TEST(ArtWriter, Empty)
{
	// Write to File
	const std::string testFilename("Sprite/data/test.prt");
	EXPECT_NO_THROW(ArtFile::Write(testFilename, ArtFile()));
	XFile::DeletePath(testFilename);

	// Write to Memory
	Stream::DynamicMemoryWriter writer;
	EXPECT_NO_THROW(ArtFile::Write(writer, ArtFile()));
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
	Stream::DynamicMemoryWriter writer;

	// Check no throw if scanLine next 4 byte aligned
	EXPECT_NO_THROW(ArtFile::Write(writer, artFile));

	// Check throw if scanLine > width && < 4 byte aligned
	artFile.imageMetas[0].scanLineByteWidth = 11;
	EXPECT_THROW(ArtFile::Write(writer, artFile), std::runtime_error);

	// Check throw if scanLine > first 4 byte align
	artFile.imageMetas[0].scanLineByteWidth = 16;
	EXPECT_THROW(ArtFile::Write(writer, artFile), std::runtime_error);

	// Check throw if scanLine < width but still 4 byte aligned
	artFile.imageMetas[0].scanLineByteWidth = 8;
	EXPECT_THROW(ArtFile::Write(writer, artFile), std::runtime_error);
}

TEST_F(SimpleArtFile, Write_PaletteIndexRange) 
{
	Stream::DynamicMemoryWriter writer;

	// Check for no throw when ImageMeta.paletteIndex is within palette container's range
	EXPECT_NO_THROW(ArtFile::Write(writer, artFile));

	artFile.palettes.clear();

	// Check for throw due to ImageMeta.paletteIndex outside of palette container's range
	EXPECT_THROW(ArtFile::Write(writer, artFile), std::runtime_error);
}

TEST_F(SimpleArtFile, Write_PaletteColors)
{
	const uint8_t red = 255;
	const uint8_t blue = 0;
	artFile.palettes[0][0] = Color{ red, 0, blue, 0 };

	Stream::DynamicMemoryWriter writer;

	EXPECT_NO_THROW(ArtFile::Write(writer, artFile));

	// Check ArtFile palette remains unchanged after write
	EXPECT_EQ(red, artFile.palettes[0][0].red);
	EXPECT_EQ(blue, artFile.palettes[0][0].blue);

	// Check ArtFile palette written to disk properly
	auto reader = writer.GetReader();
	artFile = ArtFile::Read(reader);
	EXPECT_EQ(red, artFile.palettes[0][0].red);
	EXPECT_EQ(blue, artFile.palettes[0][0].blue);
}
