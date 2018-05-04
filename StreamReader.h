#pragma once

#include <fstream>
#include <string>

class StreamReader {
public:
	virtual ~StreamReader() = 0;
	virtual void Read(char* buffer, size_t size) = 0;
};

class SeekableStreamReader : public StreamReader {
public:
	// Change position forward or backword in buffer.
	virtual void SeekRelative(int offset) = 0;
};

class FileStreamReader : public SeekableStreamReader {
public:
	FileStreamReader(std::string fileName);
	~FileStreamReader();
	void Read(char* buffer, size_t size);
	// Change position forward or backword in buffer.
	void SeekRelative(int offset);

private:
	std::ifstream file;
};


class MemoryStreamReader : public SeekableStreamReader {
public:
	MemoryStreamReader(char* buffer, size_t size);
	void Read(char* buffer, size_t size);
	void SeekRelative(int offset);

private:
	char* streamBuffer;
	size_t streamSize;
	size_t offset;
};
