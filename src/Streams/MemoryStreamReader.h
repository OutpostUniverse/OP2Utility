#pragma once

#include "SeekableStreamReader.h"
#include <cstddef>
#include <cstdint>

namespace Stream
{
	class MemoryStreamReader : public SeekableStreamReader {
	public:
		MemoryStreamReader(const void* const buffer, std::size_t size);

		std::size_t ReadPartial(void* buffer, std::size_t size) noexcept override;

		// SeekableStreamReader methods
		uint64_t Length() override;
		uint64_t Position() override;
		void Seek(uint64_t position) override;
		void SeekRelative(int64_t offset) override;

		MemoryStreamReader Slice(uint64_t sliceLength);
		MemoryStreamReader Slice(uint64_t sliceStartPosition, uint64_t sliceLength) const;

	protected:
		// StreamReader methods
		void ReadImplementation(void* buffer, std::size_t size) override;

	private:
		const char* const streamBuffer;
		const std::size_t streamSize;
		std::size_t position;
	};
}
