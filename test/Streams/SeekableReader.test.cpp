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

	virtual ~SimpleSeekableReader() { delete seekableReader; }

	Stream::SeekableReader* const seekableReader;
	const unsigned int size = 5;
};

using testing::Types;

typedef Types<Stream::MemoryReader, Stream::FileReader> SeekableStreamImplementations;

TYPED_TEST_CASE(SimpleSeekableReader, SeekableStreamImplementations); 

TYPED_TEST(SimpleSeekableReader, SeekRelativeOutOfBoundsBeginningPreservesPosition) {
	auto position = this->seekableReader->Position();
	EXPECT_THROW(this->seekableReader->SeekRelative(-1), std::runtime_error);
	EXPECT_EQ(position, this->seekableReader->Position());
}

// Then use TYPED_TEST(TestCaseName, TestName) to define a typed test,
// similar to TEST_F.
TYPED_TEST(SimpleSeekableReader, StreamPositionUpdatesOnReadVer2) {
	std::array<char, 6> destinationBuffer;

	EXPECT_EQ(this->seekableReader->Position(), 0);

	this->seekableReader->Read(&destinationBuffer[0], 1);
	EXPECT_EQ(this->seekableReader->Position(), 1);
}

TYPED_TEST(SimpleSeekableReader, StreamSizeMatchesInitialization) {
	EXPECT_EQ(this->seekableReader->Length(), this->size);
}
