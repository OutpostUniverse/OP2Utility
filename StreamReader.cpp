#include "StreamReader.h"
#include <iostream>
#include <fstream>

// Defers calls to C++ standard library methods
FileStreamReader::FileStreamReader(std::string filename) {
	file = ifstream(filename, ios::in | ios::binary);
	
	if (!file.is_open())
		throw exception("Map file could not be opened.");
}

FileStreamReader::~FileStreamReader() {
	file.close();
}

void FileStreamReader::read(char* buffer, size_t size) {
	file.read(buffer, size);
}

void FileStreamReader::ignore(size_t size) {
	file.ignore(size);
}


MemoryStreamReader::MemoryStreamReader(char* buffer, size_t size) {
	streamBuffer = buffer;
	streamSize = size;
	offset = 0;
}

void MemoryStreamReader::read(char* buffer, size_t size) {
	if (offset + size > streamSize)
		throw exception("Size to read exceeds remaining size of buffer.");

	memcpy(buffer, streamBuffer + offset, size);
	offset += size;
}

void MemoryStreamReader::ignore(size_t size)
{
	if (offset + size > streamSize)
		throw exception("Size to ignore exceeds remaining size of buffer.");

	offset += size;
}