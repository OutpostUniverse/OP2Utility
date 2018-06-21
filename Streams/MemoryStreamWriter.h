#pragma once

#include "SeekableStreamWriter.h"
#include <cstddef>
#include <cstdint>

class MemoryStreamWriter : public SeekableStreamWriter
{
public:
	// buffer: where data will be written to.
	// size: Amount of space allocated in the buffer for writing into.
	MemoryStreamWriter(void* buffer, std::size_t size);

	// SeekableStreamWriter methods
	uint64_t Length() override;
	uint64_t Position() override;
	void Seek(uint64_t offset) override;
	void SeekRelative(int64_t offset) override;

protected:
	void WriteImplementation(const void* buffer, std::size_t size) override;

private:
	// Memory location to write data into.
	char* const streamBuffer;

	// Size of the provided streamBuffer.
	const std::size_t streamSize;

	// Current location in the streamBuffer.
	std::size_t offset;
};
