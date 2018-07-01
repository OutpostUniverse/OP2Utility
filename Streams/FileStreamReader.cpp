#include "FileStreamReader.h"
#include "FileSliceReader.h"
#include <stdexcept>

// Defers calls to C++ standard library methods
FileStreamReader::FileStreamReader(std::string filename) : 
	filename(filename),
	file(filename, std::ios::in | std::ios::binary)
{
	if (!file.is_open()) {
		throw std::runtime_error("Could not open file: " + filename);
	}
}

FileStreamReader::~FileStreamReader() {
	file.close();
}

void FileStreamReader::ReadImplementation(void* buffer, std::size_t size) {
	file.read(static_cast<char*>(buffer), size);
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

std::unique_ptr<SeekableStreamReader> FileStreamReader::Slice(uint64_t sliceLength) 
{
	auto slice = Slice(Position(), sliceLength);

	// Wait until slice is successfully created before seeking forward.
	SeekRelative(sliceLength);

	return slice;
}

std::unique_ptr<SeekableStreamReader> FileStreamReader::Slice(uint64_t sliceStartPosition, uint64_t sliceLength) {
	return std::make_unique<FileSliceReader>(filename, sliceStartPosition, sliceLength);
}
