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
	 * An internal memory buffer is managed by DynamicMemoryWriter, which grows as needed to accommodate new data.
	 * This is in contrast to MemoryWriter, which is a view to a non-owned pre-set sized buffer, which can not grow.
	 *
	 * \note Adding additional data to the stream after calling GetReader may invalidate existing readers.
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
