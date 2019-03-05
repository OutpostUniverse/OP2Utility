#include "DynamicMemoryWriter.h"
#include <cstring>    // memcpy
#include <limits>
#include <stdexcept>

namespace Stream
{
	DynamicMemoryWriter::DynamicMemoryWriter() {
	}

	DynamicMemoryWriter::DynamicMemoryWriter(std::size_t preallocateSize) {
		streamBuffer.reserve(preallocateSize);
	}

	void DynamicMemoryWriter::WriteImplementation(const void* buffer, std::size_t size)
	{
		auto streamSize = streamBuffer.size();
		streamBuffer.resize(streamSize + size);
		std::memcpy(streamBuffer.data() + streamSize, buffer, size);
	}

	uint64_t DynamicMemoryWriter::Length()
	{
		return streamBuffer.size();
	}

	uint64_t DynamicMemoryWriter::Position()
	{
		return streamBuffer.size();
	}

	void DynamicMemoryWriter::SeekForward(uint64_t offset)
	{
		auto streamSize = streamBuffer.size();
		if (offset > std::numeric_limits<SizeType>::max() - streamSize) {
			throw std::runtime_error("Seek forward beyond stream size limit");
		}
		// Zero fill to new size
		streamBuffer.resize(static_cast<SizeType>(streamSize + offset), 0);
	}

	void DynamicMemoryWriter::SeekBackward(uint64_t offset)
	{
		auto streamSize = streamBuffer.size();
		if (offset > streamSize) {
			throw std::runtime_error("Seek backward before beginning of stream");
		}
		streamBuffer.resize(static_cast<SizeType>(streamSize - offset), 0);
	}

	void DynamicMemoryWriter::Seek(uint64_t offset)
	{
		streamBuffer.resize(static_cast<SizeType>(offset), 0);
	}

	MemoryReader DynamicMemoryWriter::GetReader() {
		return MemoryReader(streamBuffer.data(), streamBuffer.size());
	}
}
