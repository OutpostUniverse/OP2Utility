#pragma once

#include "Stream/BidirectionalReader.h"
#include <gtest/gtest.h>

using namespace OP2Utility;

template <class T>
T CreateBidirectionalReader();

// Define a test fixture class template
template <class T>
class SimpleBidirectionalReader : public testing::Test {
protected:
	// The ctor calls the factory function to create a reader implemented by T
	SimpleBidirectionalReader() : reader(CreateBidirectionalReader<T>()) {}

	T reader;
	const unsigned int size = 5;
};

TYPED_TEST_SUITE_P(SimpleBidirectionalReader);

TYPED_TEST_P(SimpleBidirectionalReader, Seek) {
	EXPECT_NO_THROW(this->reader.Seek(1));
	EXPECT_EQ(1, reader.Position());

	EXPECT_NO_THROW(this->reader.SeekBackward(1));
	EXPECT_EQ(0, reader.Position());
}

TYPED_TEST_P(SimpleBidirectionalReader, SeekRelativeOutOfBoundsBeginningPreservesPosition) {
	auto position = this->reader.Position();
	EXPECT_THROW(this->reader.SeekBackward(1), std::runtime_error);
	EXPECT_EQ(position, this->reader.Position());
}

// Then use TYPED_TEST(TestCaseName, TestName) to define a typed test,
// similar to TEST_F.
TYPED_TEST_P(SimpleBidirectionalReader, StreamPositionUpdatesOnRead) {
	char destinationBuffer;

	EXPECT_EQ(0u, this->reader.Position());

	this->reader.Read(destinationBuffer);
	EXPECT_EQ(1u, this->reader.Position());
}

TYPED_TEST_P(SimpleBidirectionalReader, StreamSizeMatchesInitialization) {
	EXPECT_EQ(this->size, this->reader.Length());
}

TYPED_TEST_P(SimpleBidirectionalReader, SeekBeginningAndEnd) {
	this->reader.SeekEnd();
	ASSERT_EQ(this->size, this->reader.Position());

	this->reader.SeekBeginning();
	EXPECT_EQ(0u, this->reader.Position());
}

REGISTER_TYPED_TEST_SUITE_P(SimpleBidirectionalReader,
	Seek,
	SeekRelativeOutOfBoundsBeginningPreservesPosition,
	StreamPositionUpdatesOnRead,
	StreamSizeMatchesInitialization,
	SeekBeginningAndEnd
);
