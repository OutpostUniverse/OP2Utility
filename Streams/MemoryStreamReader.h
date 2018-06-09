#pragma once

#include "SeekableStreamReader.h"
#include <cstdint>

class MemoryStreamReader : public SeekableStreamReader {
public:
	MemoryStreamReader(void* buffer, size_t size);

	// StreamReader methods
	void Read(void* buffer, size_t size) override;

	// Inline templated convenience methods, to easily read arbitrary data types
	template<typename T> inline void Read(T& object) {
		Read((char*)&object, sizeof(object));
	}

	// SeekableStreamReader methods
	uint64_t Length() override;
	uint64_t Position() override;
	void Seek(uint64_t position) override;
	void SeekRelative(int64_t offset) override;

private:
	char* streamBuffer;
	size_t streamSize;
	size_t position;
};
