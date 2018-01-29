#include "StreamReader.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <stdexcept>

using namespace std;

// Defers calls to C++ standard library methods
FileStreamReader::FileStreamReader(std::string filename) {
	file = ifstream(filename, ios::in | ios::binary);

	if (!file.is_open()) {
		throw runtime_error("File could not be opened.");
	}
}

FileStreamReader::~FileStreamReader() {
	file.close();
}

void FileStreamReader::Read(char* buffer, size_t size) {
	file.read(buffer, size);
}

void FileStreamReader::Ignore(size_t size) {
	file.ignore(size);
}


MemoryStreamReader::MemoryStreamReader(char* buffer, size_t size) {
	streamBuffer = buffer;
	streamSize = size;
	offset = 0;
}

void MemoryStreamReader::Read(char* buffer, size_t size) 
{
	if (offset + size > streamSize) {
		throw runtime_error("Size of bytes to read exceeds remaining size of buffer.");
	}

	memcpy(buffer, streamBuffer + offset, size);
	offset += size;
}

void MemoryStreamReader::Ignore(size_t size)
{
	if (offset + size > streamSize) {
		throw runtime_error("Size of bytes to ignore exceeds remaining size of buffer.");
	}

	offset += size;
}
