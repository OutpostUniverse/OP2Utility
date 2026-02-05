#include "Reader.test.h"
#include "BidirectionalReader.test.h"
#include "Stream/MemoryReader.h"
#include <array>
#include <string>
#include <stdexcept>

using namespace OP2Utility;

template <>
Stream::MemoryReader CreateReader<Stream::MemoryReader>() {
	std::array<char, 5> buffer{ 't', 'e', 's', 't', '!' };
	return Stream::MemoryReader(buffer.data(), buffer.size() * sizeof(char));
}

INSTANTIATE_TYPED_TEST_SUITE_P(BasicMemoryReader, BasicReaderTests, Stream::MemoryReader);


template <>
Stream::MemoryReader CreateBidirectionalReader<Stream::MemoryReader>() {
	std::array<char, 5> buffer{ 't', 'e', 's', 't', '!' };
	return Stream::MemoryReader(buffer.data(), buffer.size() * sizeof(char));
}

INSTANTIATE_TYPED_TEST_SUITE_P(MemoryReader, SimpleBidirectionalReader, Stream::MemoryReader);


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
	EXPECT_EQ(0u, stream.Length());
	EXPECT_EQ(0u, stream.Position());

	// Seek to current position (should not cause error)
	ASSERT_NO_THROW(stream.Seek(0));
	ASSERT_NO_THROW(stream.SeekForward(0));
	ASSERT_NO_THROW(stream.SeekBackward(0));

	// Read 0 bytes
	ASSERT_NO_THROW(stream.Peek(nullptr, 0));
	ASSERT_NO_THROW(stream.Read(nullptr, 0));
	EXPECT_EQ(0u, stream.ReadPartial(nullptr, 0));
}


class SimpleMemoryReader : public ::testing::Test {
public:
	SimpleMemoryReader() : stream(buffer.data(), buffer.size()) {}
protected:
	const std::array<char, 5> buffer{ 't', 'e', 's', 't', '!' };
	Stream::MemoryReader stream;
};

TEST_F(SimpleMemoryReader, Peek) {
	char c;
	ASSERT_NO_THROW(stream.Peek(c));
	ASSERT_EQ(0u, stream.Position());
	ASSERT_EQ(buffer[0], c);
}

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

TEST(MemoryReader, ReadNullTerminatedStringUnbounded)
{
	constexpr std::array<char, 5> terminatedBuffer{ 'n', 'u', 'l', 'l', '\0' };
	Stream::MemoryReader reader(&terminatedBuffer[0], terminatedBuffer.size());

	// Test unbounded read
	EXPECT_EQ("null", reader.ReadNullTerminatedString());
	// Stream is advanced by string length + null terminator
	EXPECT_EQ(5u, reader.Position());
}

TEST(MemoryReader, ReadNullTerminatedStringBounded)
{
	constexpr std::array<char, 5> terminatedBuffer{ 'n', 'u', 'l', 'l', '\0' };
	Stream::MemoryReader reader(&terminatedBuffer[0], terminatedBuffer.size());

	// Test bounded read with maxCount characters
	EXPECT_EQ("nu", reader.ReadNullTerminatedString(2));
	// Stream is advanced by read length (no null terminator was seen)
	EXPECT_EQ(2u, reader.Position());
}

TEST(MemoryReader, ReadNonNullTerminatedString)
{
	constexpr std::array<char, 5> nonTerminatedBuffer{ 'n', 'o', 'p', 'e', '!' };
	Stream::MemoryReader reader(&nonTerminatedBuffer[0], nonTerminatedBuffer.size());

	// Unbounded read of non-terminated string will throw when end of stream is reached
	EXPECT_THROW(reader.ReadNullTerminatedString(), std::runtime_error);
}
