#include "Streams/MemoryReader.h"
#include <gtest/gtest.h>
#include <array>

// Simple test

TEST(MemoryStreamReaderTest, ZeroSizeStreamHasSafeOperations) {
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

TEST_F(EmptyMemoryStreamReader, ZeroSizeStreamHasSafeOperations) {
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


class SimpleMemoryStreamReader : public ::testing::Test {
public:
	SimpleMemoryStreamReader() : stream(buffer.data(), buffer.size()) {}
protected:
	const std::array<char, 5> buffer{ 't', 'e', 's', 't', '!' };
	Stream::MemoryReader stream;
};

TEST_F(SimpleMemoryStreamReader, SeekOutOfBoundsEndPreservesPosition) {
	// Check for strong exception safety
	auto position = stream.Position();
	EXPECT_THROW(stream.Seek(6), std::runtime_error);
	EXPECT_EQ(position, stream.Position());
}

TEST_F(SimpleMemoryStreamReader, ReadOutOfBoundsPreservesPosition) {
	std::array<char, 6> destinationBuffer;
	auto position = stream.Position();
	EXPECT_THROW(stream.Read(destinationBuffer), std::runtime_error);
	EXPECT_EQ(position, stream.Position());
}

TEST_F(SimpleMemoryStreamReader, SeekRelativeOutOfBoundsEndPreservesPosition) {
	auto position = stream.Position();
	EXPECT_THROW(stream.SeekRelative(6), std::runtime_error);
	EXPECT_EQ(position, stream.Position());
}

TEST_F(SimpleMemoryStreamReader, SeekRelativeOutOfBoundsBeginningPreservesPosition) {
	auto position = stream.Position();
	EXPECT_THROW(stream.SeekRelative(-1), std::runtime_error);
	EXPECT_EQ(position, stream.Position());
}

TEST_F(SimpleMemoryStreamReader, StreamSizeMatchesInitialization) {
	EXPECT_EQ(stream.Length(), buffer.size());
}

TEST_F(SimpleMemoryStreamReader, StreamPositionUpdatesOnRead) {
	std::array<char, 6> destinationBuffer;

	EXPECT_EQ(stream.Position(), 0);

	stream.Read(&destinationBuffer[0], 1);
	EXPECT_EQ(stream.Position(), 1);
}
