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

void MemoryStreamReader::Seek(uint64_t position) {
	if (position >= streamSize) {
		throw std::runtime_error("Change in offset places read position outside bounds of buffer.");
	}

	// position is checked against size of streamSize, which cannot exceed SIZE_MAX (max size of size_t)
	this->position = static_cast<size_t>(position);
}

void MemoryStreamReader::SeekRelative(int64_t offset)
{
	// Checking if offset goes below 0 is unnecessary. Arithmetic on a signed and unsigned number results 
	// in a signed number that will wraparound to a large positive and be caught.
	if (this->position + offset > streamSize) {
		throw std::runtime_error("Change in offset places read position outside bounds of buffer.");
	}

	// offset is checked against size of streamSize, which cannot exceed SIZE_MAX (max size of size_t)
	this->position += static_cast<size_t>(offset); 
}
