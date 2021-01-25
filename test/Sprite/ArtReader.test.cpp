#include "../src/Sprite/ArtFile.h"
#include "../src/Stream/DynamicMemoryWriter.h"
#include <gtest/gtest.h>

using namespace OP2Utility;

TEST(ArtReader, ReadMissingFile) {
	EXPECT_THROW(ArtFile::Read("MissingFile.prt"), std::runtime_error);

	// Check if filename is an empty string
	EXPECT_THROW(ArtFile::Read(""), std::runtime_error);
}

TEST(ArtReader, ReadEmptyFile) {
	EXPECT_THROW(ArtFile::Read("Sprite/data/Empty.prt"), std::runtime_error);
}

TEST(ArtReader, ReadStream) {
	// We want a simple valid source to load from, so we will create one by first writing it
	Stream::DynamicMemoryWriter writer;
	ArtFile().Write(writer);

	// Read from stream as lvalue
	auto reader = writer.GetReader();
	EXPECT_NO_THROW(auto artFile = ArtFile::Read(reader));

	// Read from stream as rvalue (unnamed temporary object)
	EXPECT_NO_THROW(auto artFile = ArtFile::Read(writer.GetReader()));
}
