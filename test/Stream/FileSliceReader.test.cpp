#include "BiDirectionalSeekableReader.test.h"
#include "Stream/FileSliceReader.h"

template <>
Stream::FileSliceReader CreateSeekableReader<Stream::FileSliceReader>() {
	Stream::FileReader fileReader("Stream/data/SimpleStream.txt");
	return fileReader.Slice(5);
}

INSTANTIATE_TYPED_TEST_CASE_P(FileSliceReader, SimpleSeekableReader, Stream::FileSliceReader);
