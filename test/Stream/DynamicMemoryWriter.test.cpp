#include "Stream/DynamicMemoryWriter.h"
#include <gtest/gtest.h>
#include <cstdint>
#include <array>


TEST(DynamicMemoryWriter, LengthAutoExpands) {
	const std::array<uint8_t, 4> data = {0, 1, 2, 3};
	Stream::DynamicMemoryWriter writer;

	// Initially stream has 0 length
	EXPECT_EQ(0, writer.Length());

	// Length grows as data is added
	EXPECT_NO_THROW(writer.Write(data));
	EXPECT_EQ(4, writer.Length());

	// Seeking forward past the end expands the stream
	EXPECT_NO_THROW(writer.SeekForward(1));
	EXPECT_EQ(5, writer.Length());

	// Additional data is added after the expansion gap
	EXPECT_NO_THROW(writer.Write(data));
	EXPECT_EQ(9, writer.Length());
}

TEST(DynamicMemoryWriter, ReadsBackStoredData) {
	const std::array<uint8_t, 4> data = {0, 1, 2, 3};
	Stream::DynamicMemoryWriter writer;

	// Add data to stream
	EXPECT_NO_THROW(writer.Write(data));
	// Seeking forward 0 fills
	EXPECT_NO_THROW(writer.SeekForward(1));
	// Write more data after the gap
	EXPECT_NO_THROW(writer.Write(data));

	// Get stream to read back data
	auto reader = writer.GetReader();
	std::array<uint8_t, 4> readData;
	uint8_t readDataByte;

	// Read data back, and ensure it matches up
	EXPECT_NO_THROW(reader.Read(readData));
	EXPECT_EQ(data, readData);
	EXPECT_NO_THROW(reader.Read(readDataByte));
	EXPECT_EQ(0, readDataByte);
	EXPECT_NO_THROW(reader.Read(readData));
	EXPECT_EQ(data, readData);
}
