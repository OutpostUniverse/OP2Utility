#include "FileStreamWriter.h"
#include <stdexcept>

FileStreamWriter::FileStreamWriter(const std::string& filename)
{
	fileStream.open(filename.c_str(), std::ios::trunc | std::ios::out | std::ios::binary);

	if (!fileStream.is_open()) {
		throw std::runtime_error("File could not be opened.");
	}
}

FileStreamWriter::~FileStreamWriter() {
	fileStream.close();
}

void FileStreamWriter::Write(const void* buffer, std::size_t size)
{
	fileStream.write(static_cast<const char*>(buffer), size);
}

uint64_t FileStreamWriter::Length()
{
	auto currentPosition = fileStream.tellp();  // Record current position
	fileStream.seekp(0, std::ios_base::end);    // Seek to end of file
	auto length = fileStream.tellp();   // Record current position (length of file)
	fileStream.seekp(currentPosition);  // Restore position
	return length;
}

uint64_t FileStreamWriter::Position()
{
	return fileStream.tellp();  // Return the current put pointer
}

void FileStreamWriter::Seek(uint64_t offset)
{
	fileStream.seekp(offset);
}

void FileStreamWriter::SeekRelative(int64_t offset)
{
	fileStream.seekp(offset, std::ios_base::cur);
}
