#include "FileStreamReader.h"
#include "FileSliceReader.h"
#include <stdexcept>

// Defers calls to C++ standard library methods
FileStreamReader::FileStreamReader(std::string filename) : 
	filename(filename),
	file(filename, std::ios::in | std::ios::binary)
{
	Initialize();
}

FileStreamReader::FileStreamReader(const FileStreamReader& fileStreamReader) :
	filename(fileStreamReader.filename),
	file(fileStreamReader.filename, std::ios::in | std::ios::binary)
{
	Initialize();
}

void FileStreamReader::Initialize()
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

std::size_t FileStreamReader::ReadPartial(void* buffer, std::size_t size) {
	file.read(static_cast<char*>(buffer), size);
	// Note: number of unformatted bytes read, up to size, must fit within a size_t
	return static_cast<std::size_t>(file.gcount());
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

FileSliceReader FileStreamReader::Slice(uint64_t sliceLength) 
{
	FileSliceReader slice = Slice(Position(), sliceLength);

	// Wait until slice is successfully created before seeking forward.
	SeekRelative(sliceLength);

	return slice;
}

FileSliceReader FileStreamReader::Slice(uint64_t sliceStartPosition, uint64_t sliceLength) const {
	return FileSliceReader(filename, sliceStartPosition, sliceLength);
}
