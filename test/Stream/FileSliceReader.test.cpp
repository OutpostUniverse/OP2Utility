#include "BiDirectionalSeekableReader.test.h"
#include "Stream/FileSliceReader.h"

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
