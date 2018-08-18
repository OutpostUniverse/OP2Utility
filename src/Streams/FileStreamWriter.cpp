#include "FileStreamWriter.h"
#include <stdexcept>

namespace Stream
{
	FileStreamWriter::FileStreamWriter(const std::string& filename) :
		filename(filename),
		file(filename, std::ios::trunc | std::ios::out | std::ios::binary)
	{
		if (!file.is_open()) {
			throw std::runtime_error("File could not be opened.");
		}
	}

	FileStreamWriter::~FileStreamWriter() {
		file.close();
	}

	void FileStreamWriter::WriteImplementation(const void* buffer, std::size_t size)
	{
		file.write(static_cast<const char*>(buffer), size);
	}

	uint64_t FileStreamWriter::Length()
	{
		auto currentPosition = file.tellp();  // Record current position
		file.seekp(0, std::ios_base::end);    // Seek to end of file
		auto length = file.tellp();   // Record current position (length of file)
		file.seekp(currentPosition);  // Restore position
		return length;
	}

	uint64_t FileStreamWriter::Position()
	{
		return file.tellp();  // Return the current put pointer
	}

	void FileStreamWriter::Seek(uint64_t offset)
	{
		file.seekp(offset);
	}

	void FileStreamWriter::SeekRelative(int64_t offset)
	{
		file.seekp(offset, std::ios_base::cur);
	}
}
