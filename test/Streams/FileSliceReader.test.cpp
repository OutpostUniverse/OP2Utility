#include "SeekableReader.test.h"
#include "Streams/FileSliceReader.h"

template <>
Stream::FileSliceReader CreateSeekableReader<Stream::FileSliceReader>() {
	Stream::FileReader fileReader("test/Streams/SimpleStream.txt");
	return fileReader.Slice(5);
}

INSTANTIATE_TYPED_TEST_CASE_P(FileSliceReader, SimpleSeekableReader, Stream::FileSliceReader);
