#pragma once

#include "BidirectionalWriter.h"
#include "MemoryReader.h"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace Stream
{
	/**
	 * \brief Memory backed stream, which grows dynamically in size as data is added
	 *
	 * This stream can be used as a destination when the output size is not known exactly ahead of time.
	 * Written data can later be accessed by calling GetReader to retrieve a MemoryReader for the written data.
	 * The DynamicMemoryWriter is particularly useful when writing test code for stream serializaation,
	 * as it does not need to write to disk, nor cleanup temporary files, during round-trip testing.
	 *
	 * An internal memory buffer is managed by DynamicMemoryWriter, which grows as needed to accommodate new data.
	 * This is in contrast to MemoryWriter, which is a view to a non-owned pre-set sized buffer, which can not grow.
	 *
	 * \note Adding additional data to the stream after calling GetReader invalidates existing readers.
	 * (They may be left pointing at old memory after a re-allocation).
	 */
	class DynamicMemoryWriter : public BidirectionalWriter
	{
	public:
		DynamicMemoryWriter();
		DynamicMemoryWriter(std::size_t preallocateSize);

		uint64_t Length() override;
		uint64_t Position() override;

		// SeekForward will set a new stream size and 0 fill the buffer gap
		void SeekForward(uint64_t offset) override;
		// SeekBackward will set a new stream size and truncate the stream
		void SeekBackward(uint64_t offset) override;
		// Seek will set a new stream size (either by 0 filling or by truncating)
		void Seek(uint64_t position) override;

		// Get read access to the written data
		MemoryReader GetReader();

	protected:
		void WriteImplementation(const void* buffer, std::size_t size) override;

	private:
		std::vector<uint8_t> streamBuffer;

		using SizeType = decltype(streamBuffer)::size_type;
	};
}
