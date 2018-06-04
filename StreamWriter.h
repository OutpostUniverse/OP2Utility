#pragma once

#include <fstream>
#include <string>

class StreamWriter
{
public:
	virtual ~StreamWriter() = 0;
	virtual void Write(const char* buffer, size_t size) = 0;

	// Inline templated convenience methods, to easily read arbitrary data types
	template<typename T> inline void Write(T& object) {
		Write((char*)&object, sizeof(object));
	}
};

class SeekableStreamWriter : public StreamWriter
{
public:
	// Change position forward or backword in buffer.
	virtual void SeekRelative(int offset) = 0;
};

class FileStreamWriter : public SeekableStreamWriter
{
public:
	FileStreamWriter(const std::string& filename);
	~FileStreamWriter();
	void Write(const char* buffer, size_t size);

	// Change position forward or backword in buffer.
	void SeekRelative(int offset);

private:
	std::fstream fileStream;
};

class MemoryStreamWriter : public SeekableStreamWriter
{
public:
	// buffer: where data will be written to.
	// size: Amount of space allocated in the buffer for writing into.
	MemoryStreamWriter(char* buffer, size_t size);

	void Write(const char* buffer, size_t size);

	// Change position forward or backword in buffer.
	void SeekRelative(int offset);

private:
	// Memory location to write data into.
	char* streamBuffer;

	// Size of the provided streamBuffer.
	size_t streamSize;

	// Current location in the streamBuffer.
	size_t offset;
};
