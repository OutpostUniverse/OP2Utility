#include "Streams/MemoryReader.h"
#include <gtest/gtest.h>
#include <array>

// Simple test

TEST(MemoryStreamReaderTest, EmptyStream) {
  Stream::MemoryReader stream(nullptr, 0);
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


// Test with fixture

// Setup fixture
class EmptyMemoryStreamReader : public ::testing::Test {
public:
  EmptyMemoryStreamReader() : stream(nullptr, 0) {}
protected:
  Stream::MemoryReader stream;
};

TEST_F(EmptyMemoryStreamReader, EmptySize) {
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

// Setup fixture
class IntMemoryStreamReader : public ::testing::Test {
public:
	IntMemoryStreamReader() : stream(buffer.data(), buffer.size()) {}
protected:
	Stream::MemoryReader stream;
	const std::array<int, 5> buffer{ 0, 1, 2, 3, 4 };
};

TEST_F(IntMemoryStreamReader, SeekOutOfBounds) {
	EXPECT_THROW(stream.Seek(6), std::runtime_error);
}

TEST_F(IntMemoryStreamReader, ReadOutOfBounds) {
	std::array<int, 6> destinationBuffer;
	EXPECT_THROW(stream.Read(destinationBuffer), std::runtime_error);
}

TEST_F(IntMemoryStreamReader, SeekOutOfBoundsRelative) {
	// Seeking out of bounds should return a Stream::Seekable to previous condition
	EXPECT_THROW(stream.SeekRelative(6), std::runtime_error);
	EXPECT_THROW(stream.SeekRelative(-1), std::runtime_error);
}

TEST_F(IntMemoryStreamReader, StreamSize) {
	EXPECT_EQ(stream.Length(), buffer.size());
}

TEST_F(IntMemoryStreamReader, StreamPosition) {
	std::array<int, 6> destinationBuffer;

	EXPECT_EQ(stream.Position(), 0);

	stream.Read(&destinationBuffer[0], 1);
	EXPECT_EQ(stream.Position(), 1);
}
