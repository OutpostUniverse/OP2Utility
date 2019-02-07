#include "BiDirectionalSeekableReader.test.h"
#include "Stream/ReaderSlice.h"

template <>
Stream::FileSliceReader CreateSeekableReader<Stream::FileSliceReader>() {
	Stream::FileReader fileReader("Stream/data/SimpleStream.txt");
	return fileReader.Slice(5);
}

INSTANTIATE_TYPED_TEST_CASE_P(FileSliceReader, SimpleSeekableReader, Stream::FileSliceReader);


TEST(FileSliceReader, SliceIsBoundsChecked) {
	Stream::FileReader stream("Stream/data/SimpleStream.txt");

	// Proects against overflow of the stream position
	EXPECT_THROW(stream.Slice(1, std::numeric_limits<uint64_t>::max()), std::runtime_error);
	EXPECT_THROW(stream.Slice(std::numeric_limits<uint64_t>::max(), 1), std::runtime_error);

	// Protects against overstepping the bounds of the source stream
	EXPECT_THROW(stream.Slice(0, stream.Length() + 1), std::runtime_error);
	EXPECT_THROW(stream.Slice(1, stream.Length()), std::runtime_error);
	EXPECT_THROW(stream.Slice(stream.Length(), 1), std::runtime_error);
}

TEST(FileSliceReader, SliceCanBeSliced) {
	Stream::FileReader stream("Stream/data/SimpleStream.txt");
	auto slice = stream.Slice(1, stream.Length() - 2);

	// Creating a 0 byte (empty) slice works
	EXPECT_NO_THROW(slice.Slice(0, 0));
	EXPECT_NO_THROW(slice.Slice(slice.Length(), 0));

	// Creating a full sized slice works
	EXPECT_NO_THROW(slice.Slice(0, slice.Length()));

	// Slicing in works
	EXPECT_NO_THROW(slice.Slice(0, slice.Length() - 1));  // Chop end
	EXPECT_NO_THROW(slice.Slice(1, slice.Length() - 1));  // Chop beginning
	EXPECT_NO_THROW(slice.Slice(1, slice.Length() - 2));  // Chop both sides
}

TEST(FileSliceReader, SliceOfSliceIsBoundsChecked) {
	Stream::FileReader stream("Stream/data/SimpleStream.txt");
	auto slice = stream.Slice(1, stream.Length() - 2);

	// Protects against overstepping the bounds of the original slice
	EXPECT_THROW(slice.Slice(0, slice.Length() + 1), std::runtime_error);
	EXPECT_THROW(slice.Slice(1, slice.Length()), std::runtime_error);
	EXPECT_THROW(slice.Slice(slice.Length(), 1), std::runtime_error);
}

TEST(FileSliceReader, SliceOfSliceMatchesCorrectOffset1Param) {
	Stream::FileReader stream("Stream/data/SimpleStream.txt");
	stream.Seek(1);
	auto slice1 = stream.Slice(stream.Length() - 2);
	slice1.Seek(1);
	auto slice2 = slice1.Slice(slice1.Length() - 2);

	stream.Seek(2);
	slice1.Seek(1);

	char data, data1, data2;
	stream.Read(data);
	slice1.Read(data1);
	slice2.Read(data2);

	// Reads through different slices at corresponding offsets should match up
	EXPECT_EQ(data, data1);
	EXPECT_EQ(data, data2);
}

TEST(FileSliceReader, SliceOfSliceMatchesCorrectOffset2Param) {
	Stream::FileReader stream("Stream/data/SimpleStream.txt");
	auto slice1 = stream.Slice(1, stream.Length() - 2);
	auto slice2 = slice1.Slice(1, slice1.Length() - 2);

	stream.Seek(2);
	slice1.Seek(1);

	char data, data1, data2;
	stream.Read(data);
	slice1.Read(data1);
	slice2.Read(data2);

	// Reads through different slices at corresponding offsets should match up
	EXPECT_EQ(data, data1);
	EXPECT_EQ(data, data2);
}
