#include "../src/Sprites/ArtReader.h"
#include <gtest/gtest.h>

TEST(ArtReader, MissingFile) {
	EXPECT_THROW(ArtReader::Read("MissingFile.prt"), std::runtime_error);

	// Check if filename is an empty string
	EXPECT_THROW(ArtReader::Read(""), std::runtime_error);
}

TEST(ArtReader, EmptyFile) {
	EXPECT_THROW(ArtReader::Read("Sprites/data/Empty.prt"), std::runtime_error);
}
