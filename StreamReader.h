#pragma once

#include <fstream>
#include <string>
#include <cstdint>

class StreamReader {
public:
	virtual ~StreamReader() = 0;
	virtual void Read(char* buffer, size_t size) = 0;
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

	void Read(char* buffer, size_t size);

	// Inline templated convenience methods, to easily read arbitrary data types
	template<typename T> inline void Read(T& object) {
		Read((char*)&object, sizeof(object));
	}

	uint64_t Length();
	void Seek(uint64_t position);
	void SeekRelative(int64_t offset);

private:
	std::ifstream file;
};


class MemoryStreamReader : public SeekableStreamReader {
public:
	MemoryStreamReader(char* buffer, size_t size);
	void Read(char* buffer, size_t size);

	// Inline templated convenience methods, to easily read arbitrary data types
	template<typename T> inline void Read(T& object) {
		Read((char*)&object, sizeof(object));
	}

	uint64_t Length();
	void Seek(uint64_t position);
	void SeekRelative(int64_t offset);

private:
	char* streamBuffer;
	size_t streamSize;
	size_t position;
};
