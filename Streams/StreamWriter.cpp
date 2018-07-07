#include "StreamWriter.h"
#include "SeekableStreamReader.h"
#include <vector>
#include <stdexcept>

void StreamWriter::Write(SeekableStreamReader& seekableStreamReader) {
	Write(seekableStreamReader, seekableStreamReader.Length() - seekableStreamReader.Position());
}

void StreamWriter::Write(StreamReader& streamReader, uint64_t writeLength)
{
	std::size_t bytesToRead = 0;
	uint64_t offset = 0;
	std::vector<char> buffer;
	buffer.resize(streamPassLength);

	do
	{
		bytesToRead = buffer.size();

		// Check if less than a streamPassLength of data remains for writing
		if (offset + bytesToRead > writeLength) {
			bytesToRead = static_cast<std::size_t>(writeLength - offset);
		}

		streamReader.Read(buffer.data(), bytesToRead);

		offset += bytesToRead;

		Write(buffer.data(), bytesToRead);
	} while (bytesToRead); // End loop when BytesToRead/Write is equal to 0
}

void StreamWriter::SetStreamPassLength(std::size_t streamPassLength) 
{
	if (streamPassLength == 0) {
		throw std::runtime_error("Stream pass length must be set to a value greater than 0.");
	}

	this->streamPassLength = streamPassLength;
}
