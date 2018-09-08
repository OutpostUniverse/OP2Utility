#include "../src/Sprites/ArtFile.h"
#include <gtest/gtest.h>

TEST(ArtReader, MissingFile) {
	EXPECT_THROW(ArtFile::Read("MissingFile.prt"), std::runtime_error);

	// Check if filename is an empty string
	EXPECT_THROW(ArtFile::Read(""), std::runtime_error);
}

TEST(ArtReader, EmptyFile) {
	EXPECT_THROW(ArtFile::Read("Sprites/data/Empty.prt"), std::runtime_error);
}
