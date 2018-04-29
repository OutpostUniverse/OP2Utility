#include "StreamWriter.h"
#include "XFile.h"
#include <stdexcept>

StreamWriter::~StreamWriter() { }

FileStreamWriter::FileStreamWriter(const std::string& filename, unsigned int writeSettings)
{
	if (!FlagSet(writeSettings, FileWriteFlag::Overwrite)) {
		if (XFile::PathExists(filename)) {
			throw std::runtime_error("File already exists at provided path: " + filename + ".");
		}
	}
	
	if (FlagSet(writeSettings, FileWriteFlag::DoNotCreate)) {
		if (!XFile::PathExists(filename)) {
			throw std::runtime_error("File does not exist at provided path: " + filename + ".");
		}
	}

	fileStream.open(filename.c_str(), FormatFstreamMode(writeSettings));

	if (!fileStream.is_open()) {
		throw std::runtime_error("File could not be opened.");
	}
}

FileStreamWriter::~FileStreamWriter() {
	fileStream.close();
}

void FileStreamWriter::Write(const char* buffer, size_t size)
{
	fileStream.write(buffer, size);
}

void FileStreamWriter::Seek(int offset)
{
	fileStream.seekg(offset, std::ios_base::cur);
}


MemoryStreamWriter::MemoryStreamWriter(char* buffer, size_t size)
{
	streamBuffer = buffer;
	streamSize = size;
	offset = 0;
}

void MemoryStreamWriter::Write(const char* buffer, size_t size)
{
	if (offset + size > streamSize) {
		throw std::runtime_error("Size of bytes to write exceeds remaining size of buffer.");
	}

	memcpy(streamBuffer + offset, buffer, size);
	offset += size;
}

void MemoryStreamWriter::Seek(int offset)
{
	if (this->offset + offset > streamSize || this->offset + offset < 0) {
		throw std::runtime_error("Change in offset places read position outside bounds of buffer.");
	}

	this->offset += offset;
}

// Transfer the writeSettings bitfield into an fstream _Mode bitfield 
unsigned int FileStreamWriter::FormatFstreamMode(unsigned int writeSettings) const
{
	unsigned int fstreamMode = std::ios::out;

	if (FlagSet(writeSettings, FileWriteFlag::Truncate)) {
		fstreamMode |= std::ios::trunc;
	}
	else {
		fstreamMode |= std::ios::app;
	}

	if (!FlagSet(writeSettings, FileWriteFlag::Text)) {
		fstreamMode |= std::ios::binary;
	}

	return fstreamMode;
}

bool FileStreamWriter::FlagSet(unsigned int bitfield, unsigned int flag) const
{
	return ((bitfield & flag) == flag);
}
