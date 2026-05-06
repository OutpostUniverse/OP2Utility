#include "MemoryWriter.h"
#include <cstring> //memcpy
#include <stdexcept>

namespace OP2Utility::Stream
{
	MemoryWriter::MemoryWriter(void* buffer, std::size_t size) :
		streamBuffer(static_cast<char*>(buffer)), streamSize(size), offset(0) { }

	void MemoryWriter::WriteImplementation(const void* buffer, std::size_t size)
	{
		if (offset + size > streamSize) {
			throw std::runtime_error("Size of bytes to write exceeds remaining size of buffer.");
		}

		std::memcpy(streamBuffer + offset, buffer, size);
		offset += size;
	}

	uint64_t MemoryWriter::Length()
	{
		return streamSize;
	}

	uint64_t MemoryWriter::Position()
	{
		return offset;
	}

	void MemoryWriter::Seek(uint64_t newOffset)
	{
		// Checking if newOffset goes below 0 is unnecessary. Arithmetic on a signed and unsigned number results
		// in a signed number that will wraparound to a large positive and be caught.
		if (newOffset > streamSize) {
			throw std::runtime_error("Change in offset places read position outside bounds of buffer.");
		}

		this->offset = static_cast<std::size_t>(newOffset);
	}

	void MemoryWriter::SeekForward(uint64_t forwardOffset)
	{
		Seek(this->offset + forwardOffset);
	}

	void MemoryWriter::SeekBackward(uint64_t backwardOffset)
	{
		Seek(this->offset - backwardOffset);
	}
}
