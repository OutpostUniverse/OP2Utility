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
		throw std::runtime_error("File could not be opened.");
	}
}

FileStreamReader::~FileStreamReader() {
	file.close();
}

void FileStreamReader::Read(char* buffer, size_t size) {
	file.read(buffer, size);
}

void FileStreamReader::Seek(int offset) {
	file.seekg(offset, std::ios_base::cur);
}


MemoryStreamReader::MemoryStreamReader(char* buffer, size_t size) {
	streamBuffer = buffer;
	streamSize = size;
	offset = 0;
}

void MemoryStreamReader::Read(char* buffer, size_t size) 
{
	if (offset + size > streamSize) {
		throw std::runtime_error("Size of bytes to read exceeds remaining size of buffer.");
	}

	memcpy(buffer, streamBuffer + offset, size);
	offset += size;
}

void MemoryStreamReader::Seek(int offset)
{
	if (this->offset + offset > streamSize || this->offset + offset < 0) {
		throw std::runtime_error("Change in offset places read position outside bounds of buffer.");
	}

	this->offset += offset;
}
