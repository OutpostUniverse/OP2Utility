#include "BiDirectionalSeekableReader.test.h"
#include "Stream/FileSliceReader.h"

template <>
Stream::FileSliceReader CreateSeekableReader<Stream::FileSliceReader>() {
	Stream::FileReader fileReader("Stream/data/SimpleStream.txt");
	return fileReader.Slice(5);
}

INSTANTIATE_TYPED_TEST_CASE_P(FileSliceReader, SimpleSeekableReader, Stream::FileSliceReader);


TEST(FileSliceReader, SliceBoundsAreChecked) {
	Stream::FileReader stream("Stream/data/SimpleStream.txt");

	// Proects against overflow of the stream position
	EXPECT_THROW(stream.Slice(1, std::numeric_limits<uint64_t>::max()), std::runtime_error);
	EXPECT_THROW(stream.Slice(std::numeric_limits<uint64_t>::max(), 1), std::runtime_error);

	// Protects against overstepping the bounds of the source stream
	EXPECT_THROW(stream.Slice(0, stream.Length() + 1), std::runtime_error);
	EXPECT_THROW(stream.Slice(stream.Length(), 1), std::runtime_error);
}
