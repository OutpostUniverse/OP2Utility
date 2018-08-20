#include "MemoryReader.h"
#include <iostream>
#include <cstring> //memcpy
#include <stdexcept>

namespace Stream
{
	MemoryReader::MemoryReader(const void* const buffer, std::size_t size) :
		streamBuffer(static_cast<const char* const>(buffer)), streamSize(size), position(0) { }

	void MemoryReader::ReadImplementation(void* buffer, std::size_t size)
	{
		if (position + size > streamSize) {
			throw std::runtime_error("Size of bytes to read exceeds remaining size of buffer.");
		}

		std::memcpy(buffer, streamBuffer + position, size);
		position += size;
	}

	std::size_t MemoryReader::ReadPartial(void* buffer, std::size_t size) noexcept {
		auto bytesLeft = streamSize - position;
		std::size_t bytesTransferred = (size < bytesLeft) ? size : bytesLeft;

		std::memcpy(buffer, streamBuffer + position, bytesTransferred);
		position += size;

		return bytesTransferred;
	}

	uint64_t MemoryReader::Length() {
		return streamSize;
	}

	uint64_t MemoryReader::Position() {
		return position;
	}

	void MemoryReader::Seek(uint64_t position) {
		if (position > streamSize) {
			throw std::runtime_error("Change in offset places read position outside bounds of buffer.");
		}

		// position is checked against size of streamSize, which cannot exceed SIZE_MAX (max size of std::size_t)
		this->position = static_cast<std::size_t>(position);
	}

	void MemoryReader::SeekRelative(int64_t offset)
	{
		// Cast position for testing into a uint64_t. A negative value will wrap around to a positive value.
		uint64_t positionCast = static_cast<uint64_t>(this->position) + offset;

		if (positionCast > streamSize ||
			(offset > 0 && positionCast < this->position) || // Check if offset wraps past max size.
			(offset < 0 && positionCast > this->position)) // Check if offset wraps past min size.
		{
			throw std::runtime_error("Change in offset puts read position outside bounds of buffer.");
		}

		// offset is checked against size of streamSize, which cannot exceed SIZE_MAX (max size of std::size_t)
		this->position += static_cast<std::size_t>(offset);
	}

	MemoryReader MemoryReader::Slice(uint64_t sliceLength)
	{
		auto slice = Slice(Position(), sliceLength);

		// Wait until slice is successfully created before seeking forward.
		SeekRelative(sliceLength);

		return slice;
	}

	MemoryReader MemoryReader::Slice(uint64_t sliceStartPosition, uint64_t sliceLength)  const
	{
		if (sliceStartPosition > SIZE_MAX || sliceLength > SIZE_MAX) {
			throw std::runtime_error("Slice starting position and Slice length for creating a new memory stream slice must be smaller values.");
		}

		if (sliceStartPosition + sliceLength > streamSize ||
			sliceStartPosition + sliceLength < sliceStartPosition) // Check if length wraps past max size of uint64_t
		{
			throw std::runtime_error("Unable to create a slice of memory stream. Requested slice is outside bounds of underlying stream.");
		}

		return MemoryReader(&streamBuffer[sliceStartPosition], static_cast<std::size_t>(sliceLength));
	}
}
