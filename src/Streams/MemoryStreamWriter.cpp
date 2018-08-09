#include "MemoryStreamWriter.h"
#include <cstring> //memcpy
#include <stdexcept>

MemoryStreamWriter::MemoryStreamWriter(void* buffer, std::size_t size) :
	streamBuffer(static_cast<char*>(buffer)), streamSize(size), offset(0) { }

void MemoryStreamWriter::WriteImplementation(const void* buffer, std::size_t size)
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

	this->offset = static_cast<std::size_t>(offset);
}

void MemoryStreamWriter::SeekRelative(int64_t offset)
{
	Seek(this->offset + offset);
}