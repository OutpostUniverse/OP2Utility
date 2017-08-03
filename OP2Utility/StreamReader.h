#pragma once

#include <fstream>
#include <string>

using namespace std;

class StreamReader {
public:
	virtual void read(char* buffer, size_t size) = 0;
};

class SeekableStreamReader : public StreamReader {
public:
	virtual void ignore(size_t size) = 0;
};

class FileStreamReader : public SeekableStreamReader {
public:
	FileStreamReader(std::string fileName);
	~FileStreamReader();
	void read(char* buffer, size_t size);
	void ignore(size_t size);

private:
	ifstream file;
};


class MemoryStreamReader : public SeekableStreamReader {
public:
	MemoryStreamReader(char* buffer, size_t size);
	void read(char* buffer, size_t size);
	void ignore(size_t size);

private:
	char* streamBuffer;
	size_t streamSize;
	size_t offset;
};