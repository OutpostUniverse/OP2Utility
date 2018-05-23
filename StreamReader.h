#pragma once

#include <fstream>
#include <string>
#include <cstdint>

class StreamReader {
public:
	virtual ~StreamReader() = 0;
	virtual void Read(char* buffer, uint64_t size) = 0;
};

class SeekableStreamReader : public StreamReader {
public:
	// Get the size of the stream
	virtual uint64_t Length() = 0;
	
	// Seek to absolute position, given as offset from beginning of stream
	virtual void Seek(uint64_t position) = 0;
	
	// Seek by a relative amount, given as offset from current position
	virtual void SeekRelative(int64_t offset) = 0;
};

class FileStreamReader : public SeekableStreamReader {
public:
	FileStreamReader(std::string fileName);
	~FileStreamReader();

	void Read(char* buffer, uint64_t size);
	uint64_t Length();
	void Seek(uint64_t position);
	void SeekRelative(int64_t offset);

private:
	std::ifstream file;
};


class MemoryStreamReader : public SeekableStreamReader {
public:
	MemoryStreamReader(char* buffer, uint64_t size);
	void Read(char* buffer, uint64_t size);
	uint64_t Length();
	void Seek(uint64_t position);
	void SeekRelative(int64_t offset);

private:
	char* streamBuffer;
	uint64_t streamSize;
	uint64_t position;
};
