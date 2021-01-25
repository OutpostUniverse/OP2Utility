#include "BidirectionalReader.test.h"
#include "Stream/FileReader.h"
#include <array>

using namespace OP2Utility;

template <>
Stream::FileReader CreateBidirectionalReader<Stream::FileReader>() {
	return Stream::FileReader("Stream/data/SimpleStream.txt");
}

INSTANTIATE_TYPED_TEST_SUITE_P(FileReader, SimpleBidirectionalReader, Stream::FileReader);


TEST(FileReaderTest, AccessNonexistingFile) {
	EXPECT_THROW(Stream::FileReader("Stream/MissingFile.txt"), std::runtime_error);
}

TEST(FileReaderTest, ZeroSizeStreamHasSafeOperations) {
	Stream::FileReader stream("Stream/data/EmptyFile.txt");

	// Length and position
	EXPECT_EQ(0u, stream.Length());
	EXPECT_EQ(0u, stream.Position());

	// Seek to current position (should not cause error)
	ASSERT_NO_THROW(stream.Seek(0));
	ASSERT_NO_THROW(stream.SeekForward(0));
	ASSERT_NO_THROW(stream.SeekBackward(0));

	// Read 0 bytes
	ASSERT_NO_THROW(stream.Read(nullptr, 0));
	EXPECT_EQ(0u, stream.ReadPartial(nullptr, 0));
}


class SimpleFileReader : public ::testing::Test {
public:
	SimpleFileReader() : stream("Stream/data/SimpleStream.txt") {}
protected:
	Stream::FileReader stream;
};

TEST_F(SimpleFileReader, SeekRelativeOutOfBoundsBeginningPreservesPosition) {
	auto position = stream.Position();
	EXPECT_THROW(stream.SeekBackward(1), std::runtime_error);
	EXPECT_EQ(position, stream.Position());
}

TEST_F(SimpleFileReader, StreamSizeMatchesInitialization) {
	EXPECT_EQ(5u, stream.Length());
}
