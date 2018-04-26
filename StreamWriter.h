#pragma once

#include <fstream>
#include <string>

class StreamWriter
{
public:
	virtual ~StreamWriter() = 0;
	virtual void Write(const char* buffer, size_t size) = 0;
};

class FileStreamWriter : public StreamWriter
{
public:
	FileStreamWriter(const std::string& filename);
	~FileStreamWriter();
	void Write(const char* buffer, size_t size);

private:
	std::fstream fileStream;
};

class MemoryStreamWriter : public StreamWriter
{
public:
	// buffer: where data will be written to.
	// size: Amount of space allocated in the buffer for writing into.
	MemoryStreamWriter(char* buffer, size_t size);

	void Write(const char* buffer, size_t size);

private:
	// Memory location to write data into.
	char* streamBuffer;

	// Size of the provided streamBuffer.
	size_t streamSize;

	// Current location in the streamBuffer.
	size_t offset;
};
