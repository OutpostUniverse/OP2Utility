
#include "StreamWriter.h"
#include "StreamReader.h"
#include <array>

template<std::size_t BufferSize>
void StreamWriter::Write(StreamReader& streamReader) {
	std::array<char, BufferSize> buffer;
	std::size_t numBytesRead;

	do {
		// Read the input stream
		numBytesRead = streamReader.ReadPartial(buffer.data(), BufferSize);
		Write(buffer.data(), numBytesRead);
	} while (numBytesRead); // End loop when numBytesRead/Written is equal to 0
}
