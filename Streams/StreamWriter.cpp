#include "StreamWriter.h"
#include <stdexcept>
#include <cstring>

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

void FileStreamWriter::Write(const void* buffer, size_t size)
{
	fileStream.write(static_cast<const char*>(buffer), size);
}

uint64_t FileStreamWriter::Length()
{
	auto currentPosition = fileStream.tellp();  // Record current position
	fileStream.seekp(0, std::ios_base::end);    // Seek to end of file
	auto length = fileStream.tellp();   // Record current position (length of file)
	fileStream.seekp(currentPosition);  // Restore position
	return length;
}

uint64_t FileStreamWriter::Position()
{
	return fileStream.tellp();  // Return the current put pointer
}

void FileStreamWriter::Seek(uint64_t offset)
{
	fileStream.seekp(offset);
}

void FileStreamWriter::SeekRelative(int64_t offset)
{
	fileStream.seekp(offset, std::ios_base::cur);
}

MemoryStreamWriter::MemoryStreamWriter(void* buffer, size_t size)
{
	streamBuffer = static_cast<char*>(buffer);
	streamSize = size;
	offset = 0;
}

void MemoryStreamWriter::Write(const void* buffer, size_t size)
{
	if (offset + size > streamSize) {
		throw std::runtime_error("Size of bytes to write exceeds remaining size of buffer.");
	}

	std::memcpy(streamBuffer + offset, buffer, size);
	offset += size;
}

uint64_t MemoryStreamWriter::Length()
{
	return streamSize;
}

uint64_t MemoryStreamWriter::Position()
{
	return offset;
}

void MemoryStreamWriter::Seek(uint64_t offset)
{
	// Checking if offset goes below 0 is unnecessary. Arithmetic on a signed and unsigned number results 
	// in a signed number that will wraparound to a large positive and be caught.
	if (offset > streamSize) {
		throw std::runtime_error("Change in offset places read position outside bounds of buffer.");
	}

	this->offset = static_cast<size_t>(offset);
}

void MemoryStreamWriter::SeekRelative(int64_t offset)
{
	Seek(this->offset + offset);
}
