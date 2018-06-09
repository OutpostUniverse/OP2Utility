#pragma once

#include "StreamWriter.h"
#include <cstdint>

class SeekableStreamWriter : public StreamWriter
{
public:
	// Get the size of the stream
	virtual uint64_t Length() = 0;

	// Get the current position of the stream
	virtual uint64_t Position() = 0;

	// Seek to absolute position, given as offset from beginning of stream
	virtual void Seek(uint64_t offset) = 0;

	// Seek by a relative amount, given as offset from current position
	virtual void SeekRelative(int64_t offset) = 0;
};
