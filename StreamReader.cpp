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

void FileStreamReader::Read(char* buffer, uint64_t size) {
	file.read(buffer, size);
}

uint64_t FileStreamReader::Length() {
	uint64_t currentPosition = file.tellg();
	file.seekg(0, std::ios_base::end);
	uint64_t length = file.tellg();

	file.seekg(currentPosition, std::ios_base::beg);

	return length;
}

void FileStreamReader::Seek(uint64_t position) {
	file.seekg(position, std::ios_base::beg);
}

void FileStreamReader::SeekRelative(int64_t offset) {
	file.seekg(offset, std::ios_base::cur);
}


MemoryStreamReader::MemoryStreamReader(char* buffer, uint64_t size) {
	streamBuffer = buffer;
	streamSize = size;
	position = 0;
}

void MemoryStreamReader::Read(char* buffer, uint64_t size) 
{
	if (position + size > streamSize) {
		throw std::runtime_error("Size of bytes to read exceeds remaining size of buffer.");
	}

	// std::memcpy takes a _Size to copy of size_t. 
	// Depending on the system, size_t may not be able to store the max value of uint64_t. 
	// SIZE_MAX is the largest value that may be stored in size_t for the given system.
	do {
		size_t nextReadLength = SIZE_MAX;
		// Check if remaining value of size will fit in a size_t.
		if (size < SIZE_MAX) {
			nextReadLength = static_cast<size_t>(size);
		}

		std::memcpy(buffer, streamBuffer + position, nextReadLength);
		position += nextReadLength;
		size -= nextReadLength;
	} while (size != 0); // Continue reading in chunks until all data has been copied.
}

uint64_t MemoryStreamReader::Length() {
	return streamSize;
}

void MemoryStreamReader::Seek(uint64_t position) {
	if (position > this->position) {
		throw std::runtime_error("Change in offset places read position outside bounds of buffer.");
	}

	this->position = position;
}

void MemoryStreamReader::SeekRelative(int64_t offset)
{
	// Checking if offset goes below 0 is unnecessary. Arithmetic on a signed and unsigned number results 
	// in a signed number that will wraparound to a large positive and be caught.
	if (this->position + offset > streamSize) {
		throw std::runtime_error("Change in offset places read position outside bounds of buffer.");
	}

	this->position += offset;
}
