#pragma once

#include "BidirectionalWriter.h"
#include <cstddef>
#include <cstdint>

namespace Stream
{
	class MemoryWriter : public BidirectionalWriter
	{
	public:
		// buffer: where data will be written to.
		// size: Amount of space allocated in the buffer for writing into.
		MemoryWriter(void* buffer, std::size_t size);

		// SeekableWriter methods
		uint64_t Length() override;
		uint64_t Position() override;
		
		void Seek(uint64_t position) override;
		void SeekForward(uint64_t offset) override;
		void SeekBackward(uint64_t offset) override;

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
}
