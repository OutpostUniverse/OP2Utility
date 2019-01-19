#include "Reader.test.h"
#include "BiDirectionalSeekableReader.test.h"
#include "Stream/MemoryReader.h"
#include <array>


template <>
Stream::MemoryReader CreateReader<Stream::MemoryReader>() {
	std::array<char, 5> buffer{ 't', 'e', 's', 't', '!' };
	return Stream::MemoryReader(buffer.data(), buffer.size() * sizeof(char));
}

INSTANTIATE_TYPED_TEST_CASE_P(BasicMemoryReader, BasicReaderTests, Stream::MemoryReader);


template <>
Stream::MemoryReader CreateSeekableReader<Stream::MemoryReader>() {
	std::array<char, 5> buffer{ 't', 'e', 's', 't', '!' };
	return Stream::MemoryReader(buffer.data(), buffer.size() * sizeof(char));
}

INSTANTIATE_TYPED_TEST_CASE_P(MemoryReader, SimpleSeekableReader, Stream::MemoryReader);


// Simple test

TEST(MemoryStreamReaderTest, ZeroSizeStreamHasSafeOperations) {
	Stream::MemoryReader stream(nullptr, 0);
	
	// Length and position
	EXPECT_EQ(0, stream.Length());
	EXPECT_EQ(0, stream.Position());
	
	// Seek to current position (should not cause error)
	ASSERT_NO_THROW(stream.Seek(0));
	ASSERT_NO_THROW(stream.SeekForward(0));
	ASSERT_NO_THROW(stream.SeekBackward(0));
	
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
	ASSERT_NO_THROW(stream.SeekForward(0));
	ASSERT_NO_THROW(stream.SeekBackward(0));

	// Read 0 bytes
	ASSERT_NO_THROW(stream.Read(nullptr, 0));
	EXPECT_EQ(0, stream.ReadPartial(nullptr, 0));
}


class SimpleMemoryReader : public ::testing::Test {
public:
	SimpleMemoryReader() : stream(buffer.data(), buffer.size()) {}
protected:
	const std::array<char, 5> buffer{ 't', 'e', 's', 't', '!' };
	Stream::MemoryReader stream;
};

TEST_F(SimpleMemoryReader, SeekOutOfBoundsEndPreservesPosition) {
	// Check for strong exception safety
	auto position = stream.Position();
	EXPECT_THROW(stream.Seek(6), std::runtime_error);
	EXPECT_EQ(position, stream.Position());
}

TEST_F(SimpleMemoryReader, ReadOutOfBoundsPreservesPosition) {
	std::array<char, 6> destinationBuffer;
	auto position = stream.Position();
	EXPECT_THROW(stream.Read(destinationBuffer), std::runtime_error);
	EXPECT_EQ(position, stream.Position());
}

TEST_F(SimpleMemoryReader, SeekRelativeOutOfBoundsEndPreservesPosition) {
	auto position = stream.Position();
	EXPECT_THROW(stream.SeekForward(6), std::runtime_error);
	EXPECT_EQ(position, stream.Position());
}
