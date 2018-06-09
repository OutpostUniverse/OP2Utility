#pragma once

#include "SeekableStreamReader.h"
#include <string>
#include <fstream>
#include <cstdint>

class FileStreamReader : public SeekableStreamReader {
public:
	FileStreamReader(std::string fileName);

	// StreamReader methods
	~FileStreamReader() override;
	void Read(void* buffer, size_t size) override;

	// Inline templated convenience methods, to easily read arbitrary data types
	template<typename T> inline void Read(T& object) {
		Read(&object, sizeof(object));
	}

	// SeekableStreamReader methods
	uint64_t Length() override;
	uint64_t Position() override;
	void Seek(uint64_t position) override;
	void SeekRelative(int64_t offset) override;

private:
	std::ifstream file;
};
