#include "StreamReader.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <stdexcept>

StreamReader::~StreamReader() { }

// Defers calls to C++ standard library methods
FileStreamReader::FileStreamReader(std::string filename) {
	file = std::ifstream(filename, std::ios::in | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("Could not open file: " + filename);
	}
}

FileStreamReader::~FileStreamReader() {
	file.close();
}

void FileStreamReader::Read(char* buffer, size_t size) {
	file.read(buffer, size);
}

uint64_t FileStreamReader::Length() {
	auto currentPosition = file.tellg();  // Record current position
	file.seekg(0, std::ios_base::end);    // Seek to end of file
	auto streamSize = file.tellg();       // Record current position (length of file)
	file.seekg(currentPosition);          // Restore position
	return streamSize;
}

uint64_t FileStreamReader::Position() {
	return file.tellg();  // Return the current get pointer
}

void FileStreamReader::Seek(uint64_t position) {
	file.seekg(position);
}

void FileStreamReader::SeekRelative(int64_t offset) {
	file.seekg(offset, std::ios_base::cur);
}


MemoryStreamReader::MemoryStreamReader(char* buffer, size_t size) {
	streamBuffer = buffer;
	streamSize = size;
	position = 0;
}

void MemoryStreamReader::Read(char* buffer, size_t size) 
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

	// position is checked against size of streamSize, which cannot exceed SIZE_MAX (max size of size_t)
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

	// offset is checked against size of streamSize, which cannot exceed SIZE_MAX (max size of size_t)
	this->position += static_cast<size_t>(offset); 
}
