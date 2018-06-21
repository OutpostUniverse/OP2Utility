#pragma once

#include "SeekableStreamReader.h"
#include <cstddef>
#include <cstdint>

class MemoryStreamReader : public SeekableStreamReader {
public:
	MemoryStreamReader(void* buffer, std::size_t size);

	// SeekableStreamReader methods
	uint64_t Length() override;
	uint64_t Position() override;
	void Seek(uint64_t position) override;
	void SeekRelative(int64_t offset) override;

protected:
	// StreamReader methods
	void ReadImplementation(void* buffer, std::size_t size) override;

private:
	const char* const streamBuffer;
	const std::size_t streamSize;
	std::size_t position;
};
