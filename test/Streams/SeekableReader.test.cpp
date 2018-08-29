#include "SeekableReader.test.h"
#include "Streams/MemoryReader.h"
#include "Streams/FileReader.h"
#include "Streams/FileSliceReader.h"
#include <array>

using testing::Types;
typedef Types<Stream::MemoryReader, Stream::FileReader> SeekableStreamImplementations;
INSTANTIATE_TYPED_TEST_CASE_P(Prefix, SimpleSeekableReader, SeekableStreamImplementations);
