#pragma once

#include "FileWriteFlags.h"
#include <fstream>
#include <string>

class StreamWriter
{
public:
	virtual ~StreamWriter() = 0;
	virtual void Write(const char* buffer, size_t size) = 0;
};

class SeekableStreamWriter : public StreamWriter
{
public:
	// Change position forward or backword in buffer.
	virtual void Seek(int offset) = 0;
};

class FileStreamWriter : public SeekableStreamWriter
{
public:
	FileStreamWriter(const std::string& filename, unsigned int writeSettings);
	~FileStreamWriter();
	void Write(const char* buffer, size_t size);
	// Change position forward or backword in buffer.
	void Seek(int offset);

private:
	std::fstream fileStream;

	unsigned int FormatFstreamMode(unsigned int writeSettings) const;
	bool FlagSet(unsigned int bitfield, unsigned int flag) const;
};

class MemoryStreamWriter : public SeekableStreamWriter
{
public:
	// buffer: where data will be written to.
	// size: Amount of space allocated in the buffer for writing into.
	MemoryStreamWriter(char* buffer, size_t size);

	void Write(const char* buffer, size_t size);
	// Change position forward or backword in buffer.
	void Seek(int offset);

private:
	// Memory location to write data into.
	char* streamBuffer;

	// Size of the provided streamBuffer.
	size_t streamSize;

	// Current location in the streamBuffer.
	size_t offset;
};
