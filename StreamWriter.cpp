#include "StreamWriter.h"
#include <stdexcept>

StreamWriter::~StreamWriter() { }

FileStreamWriter::FileStreamWriter(const std::string& filename)
{
	fileStream.open(filename.c_str(), std::ios::trunc | std::ios::out | std::ios::binary);

	if (!fileStream.is_open()) {
		throw std::runtime_error("File could not be opened.");
	}
}

FileStreamWriter::~FileStreamWriter() {
	fileStream.close();
}

void FileStreamWriter::Write(const char* buffer, size_t size)
{
	fileStream.write(buffer, size);
}

void FileStreamWriter::SeekRelative(int offset)
{
	fileStream.seekg(offset, std::ios_base::cur);
}


MemoryStreamWriter::MemoryStreamWriter(char* buffer, size_t size)
{
	streamBuffer = buffer;
	streamSize = size;
	offset = 0;
}

void MemoryStreamWriter::Write(const char* buffer, size_t size)
{
	if (offset + size > streamSize) {
		throw std::runtime_error("Size of bytes to write exceeds remaining size of buffer.");
	}

	memcpy(streamBuffer + offset, buffer, size);
	offset += size;
}

void MemoryStreamWriter::SeekRelative(int offset)
{
	// Checking if offset goes below 0 is unnecessary. Arithmetic on a signed and unsigned number results 
	// in a signed number that will wraparound to a large positive and be caught.
	if (this->offset + offset > streamSize) {
		throw std::runtime_error("Change in offset places read position outside bounds of buffer.");
	}

	this->offset += offset;
}