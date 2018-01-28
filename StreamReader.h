#pragma once

#include <fstream>
#include <string>

class StreamReader {
public:
	virtual void Read(char* buffer, size_t size) = 0;
};

class SeekableStreamReader : public StreamReader {
public:
	virtual void Ignore(size_t size) = 0;
};

class FileStreamReader : public SeekableStreamReader {
public:
	FileStreamReader(std::string fileName);
	~FileStreamReader();
	void Read(char* buffer, size_t size);
	void Ignore(size_t size);

private:
	std::ifstream file;
};


class MemoryStreamReader : public SeekableStreamReader {
public:
	MemoryStreamReader(char* buffer, size_t size);
	void Read(char* buffer, size_t size);
	void Ignore(size_t size);

private:
	char* streamBuffer;
	size_t streamSize;
	size_t offset;
};
