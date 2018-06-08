#pragma once

#include "StreamWriter.h"
#include <string>
#include <cstdint>

class MemoryStreamWriter : public SeekableStreamWriter
{
public:
	// buffer: where data will be written to.
	// size: Amount of space allocated in the buffer for writing into.
	MemoryStreamWriter(void* buffer, size_t size);

	// StreamWriter methods
	void Write(const void* buffer, size_t size) override;

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
