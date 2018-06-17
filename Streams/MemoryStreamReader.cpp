#include "MemoryStreamReader.h"
#include <iostream>
#include <cstring> //memcpy
#include <stdexcept>

MemoryStreamReader::MemoryStreamReader(void* buffer, std::size_t size) {
	streamBuffer = static_cast<char*>(buffer);
	streamSize = size;
	position = 0;
}

void MemoryStreamReader::ReadImplementation(void* buffer, std::size_t size)
{
	if (position + size > streamSize) {
		throw std::runtime_error("Size of bytes to read exceeds remaining size of buffer.");
	}

	std::memcpy(buffer, streamBuffer + position, size);
	position += size;
}

uint64_t MemoryStreamReader::Length() {
	return streamSize;
}

uint64_t MemoryStreamReader::Position() {
	return position;
}

void MemoryStreamReader::Seek(uint64_t position) {
	if (position > streamSize) {
		throw std::runtime_error("Change in offset places read position outside bounds of buffer.");
	}

	// position is checked against size of streamSize, which cannot exceed SIZE_MAX (max size of std::size_t)
	this->position = static_cast<size_t>(position);
}

void MemoryStreamReader::SeekRelative(int64_t offset)
{
	// Cast position for testing into a uint64_t. A negative value will wrap around to a positive value.
	uint64_t positionCast = static_cast<uint64_t>(this->position) + offset;

	if (positionCast > streamSize ||
	   (offset > 0 && positionCast < this->position) || // Check if offset wraps past max size.
	   (offset < 0 && positionCast > this->position)) // Check if offset wraps past min size. 
	{
		throw std::runtime_error("Change in offset puts read position outside bounds of buffer.");
	}

	// offset is checked against size of streamSize, which cannot exceed SIZE_MAX (max size of std::size_t)
	this->position += static_cast<size_t>(offset); 
}
