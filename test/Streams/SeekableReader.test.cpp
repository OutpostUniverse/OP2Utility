#include "SeekableReader.test.h"
#include "Streams/MemoryReader.h"
#include "Streams/FileReader.h"
#include "Streams/FileSliceReader.h"
#include <array>

// Define a test fixture class template
template <class T>
class SimpleSeekableReader : public testing::Test {
protected:
	// The ctor calls the factory function to create a reader implemented by T
	SimpleSeekableReader() : seekableReader(CreateSeekableReader<T>()) {}

	T seekableReader;
	const unsigned int size = 5;
};

TYPED_TEST_CASE_P(SimpleSeekableReader);

TYPED_TEST_P(SimpleSeekableReader, SeekRelativeOutOfBoundsBeginningPreservesPosition) {
	auto position = this->seekableReader.Position();
	EXPECT_THROW(this->seekableReader.SeekRelative(-1), std::runtime_error);
	EXPECT_EQ(position, this->seekableReader.Position());
}

// Then use TYPED_TEST(TestCaseName, TestName) to define a typed test,
// similar to TEST_F.
TYPED_TEST_P(SimpleSeekableReader, StreamPositionUpdatesOnReadVer2) {
	char destinationBuffer;

	EXPECT_EQ(0, this->seekableReader.Position());

	this->seekableReader.Read(destinationBuffer);
	EXPECT_EQ(1, this->seekableReader.Position());
}

TYPED_TEST_P(SimpleSeekableReader, StreamSizeMatchesInitialization) {
	EXPECT_EQ(this->size, this->seekableReader.Length());
}

REGISTER_TYPED_TEST_CASE_P(SimpleSeekableReader,
	SeekRelativeOutOfBoundsBeginningPreservesPosition,
	StreamPositionUpdatesOnReadVer2,
	StreamSizeMatchesInitialization
);

using testing::Types;
typedef Types<Stream::MemoryReader, Stream::FileReader> SeekableStreamImplementations;
INSTANTIATE_TYPED_TEST_CASE_P(Prefix, SimpleSeekableReader, SeekableStreamImplementations);
