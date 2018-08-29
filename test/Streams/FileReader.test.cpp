#include "SeekableReader.test.h"
#include "Streams/FileReader.h"
#include <array>

template <>
Stream::FileReader CreateSeekableReader<Stream::FileReader>() {
	return Stream::FileReader("Streams/SimpleStream.txt");
}

TEST(FileReaderTest, AccessNonexistingFile) {
	EXPECT_THROW(Stream::FileReader("MissingFile.txt"), std::runtime_error);
}

TEST(FileReaderTest, ZeroSizeStreamHasSafeOperations) {
	Stream::FileReader stream("Streams/EmptyFile.txt");
	// Length and position
	EXPECT_EQ(0, stream.Length());
	EXPECT_EQ(0, stream.Position());
	// Seek to current position (should not cause error)
	ASSERT_NO_THROW(stream.Seek(0));
	ASSERT_NO_THROW(stream.SeekRelative(0));
	// Read 0 bytes
	ASSERT_NO_THROW(stream.Read(nullptr, 0));
	EXPECT_EQ(0, stream.ReadPartial(nullptr, 0));
}


class SimpleFileReader : public ::testing::Test {
public:
	SimpleFileReader() : stream("Streams/SimpleStream.txt") {}
protected:
	Stream::FileReader stream;
};

TEST_F(SimpleFileReader, SeekRelativeOutOfBoundsBeginningPreservesPosition) {
	auto position = stream.Position();
	EXPECT_THROW(stream.SeekRelative(-1), std::runtime_error);
	EXPECT_EQ(position, stream.Position());
}

TEST_F(SimpleFileReader, StreamSizeMatchesInitialization) {
	EXPECT_EQ(stream.Length(), 5);
}

TEST_F(SimpleFileReader, StreamPositionUpdatesOnRead) {
	std::array<char, 6> destinationBuffer;

	EXPECT_EQ(stream.Position(), 0);

	stream.Read(&destinationBuffer[0], 1);
	EXPECT_EQ(stream.Position(), 1);
}
