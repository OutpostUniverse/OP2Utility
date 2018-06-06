#pragma once

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
	// Get the size of the stream
	virtual uint64_t Length() = 0;
	
	// Get the current position of the stream
	virtual uint64_t Position() = 0;
	
	// Seek to absolute position, given as offset from beginning of stream
	virtual void Seek(uint64_t offset) = 0;

	// Seek by a relative amount, given as offset from current position
	virtual void SeekRelative(int64_t offset) = 0;
};

class FileStreamWriter : public SeekableStreamWriter
{
public:
	FileStreamWriter(const std::string& filename);

	// StreamWriter methods
	~FileStreamWriter() override;
	void Write(const char* buffer, size_t size) override;

	// Inline templated convenience methods, to easily read arbitrary data types
	template<typename T> inline void Write(const T& object) {
		Write((char*)&object, sizeof(object));
	}

	// SeekableStreamWriter methods
	uint64_t Length() override;
	uint64_t Position() override;
	void Seek(uint64_t offset) override;
	void SeekRelative(int64_t offset) override;

private:
	std::fstream fileStream;
};

class MemoryStreamWriter : public SeekableStreamWriter
{
public:
	// buffer: where data will be written to.
	// size: Amount of space allocated in the buffer for writing into.
	MemoryStreamWriter(char* buffer, size_t size);

	// StreamWriter methods
	void Write(const char* buffer, size_t size) override;

	// Inline templated convenience methods, to easily read arbitrary data types
	template<typename T> inline void Write(const T& object) {
		Write((char*)&object, sizeof(object));
	}

	// SeekableStreamWriter methods
	uint64_t Length() override;
	uint64_t Position() override;
	void Seek(uint64_t offset) override;
	void SeekRelative(int64_t offset) override;

private:
	// Memory location to write data into.
	char* streamBuffer;

	// Size of the provided streamBuffer.
	size_t streamSize;

	// Current location in the streamBuffer.
	size_t offset;
};
