#include "Streams/MemoryStreamReader.h"
#include <gtest/gtest.h>


// Simple test

TEST(MemoryStreamReaderTest, EmptyStream) {
  MemoryStreamReader stream(nullptr, 0);
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
  MemoryStreamReader stream;
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
