#include "SeekableReader.test.h"
#include "Streams/FileSliceReader.h"

template <>
Stream::SeekableReader* CreateSeekableReader<Stream::FileSliceReader>() {
	Stream::FileReader fileReader("Streams/SimpleStream.txt");
	return &fileReader.Slice(5);
}
