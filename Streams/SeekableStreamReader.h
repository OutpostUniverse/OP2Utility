#pragma once

#include "StreamReader.h"
#include <cstdint>
#include <memory>

class SeekableStreamReader : public StreamReader {
public:
	// Get the size of the stream
	virtual uint64_t Length() = 0;

	// Get the current position of the stream
	virtual uint64_t Position() = 0;

	// Seek to absolute position, given as offset from beginning of stream
	virtual void Seek(uint64_t position) = 0;

	// Seek by a relative amount, given as offset from current position
	virtual void SeekRelative(int64_t offset) = 0;

	// Create a slice of the stream for independent processing. Starts at current position of stream. 
	// Seeks parent stream forward the slice's length if creation is successful.
	virtual std::unique_ptr<SeekableStreamReader> Slice(uint64_t sliceLength) = 0;

	// Create a slice of the stream for independent processing.
	virtual std::unique_ptr<SeekableStreamReader> Slice(uint64_t sliceStartPosition, uint64_t sliceLength) = 0;
};
