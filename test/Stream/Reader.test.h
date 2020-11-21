#pragma once

#include "Stream/Reader.h"
#include <gtest/gtest.h>


template <class T>
T CreateReader();

template <class T>
class BasicReaderTests : public testing::Test {
protected:
	// Create instance using factory function
	BasicReaderTests() : stream(CreateReader<T>()) {}

	T stream;
};

TYPED_TEST_SUITE_P(BasicReaderTests);

TYPED_TEST_P(BasicReaderTests, CanReadZeroBytesToNull) {
	ASSERT_NO_THROW(this->stream.Read(nullptr, 0));
}

TYPED_TEST_P(BasicReaderTests, CanReadPartialZeroBytesToNull) {
	EXPECT_EQ(0u, this->stream.ReadPartial(nullptr, 0));
}

REGISTER_TYPED_TEST_SUITE_P(BasicReaderTests,
	CanReadZeroBytesToNull,
	CanReadPartialZeroBytesToNull
);
