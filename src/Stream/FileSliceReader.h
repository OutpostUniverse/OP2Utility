#pragma once

#include "FileReader.h"
#include <string>
#include <cstddef>
#include <cstdint>

namespace Stream
{
	// Opens a new file stream that is limited to reading from the provided file slice.
	class FileSliceReader : public BidirectionalSeekableReader
	{
		using WrappedStreamType = FileReader;

	public:
		FileSliceReader(const WrappedStreamType& wrappedStream, uint64_t startingOffset, uint64_t sliceLength);
		FileSliceReader(const FileSliceReader& fileSliceReader);

		std::size_t ReadPartial(void* buffer, std::size_t size) noexcept override;

		// BidirectionalSeekableReader methods
		uint64_t Length() override;
		uint64_t Position() override;

		void Seek(uint64_t position) override;
		void SeekForward(uint64_t offset) override;
		void SeekBackward(uint64_t offset) override;

		FileSliceReader Slice(uint64_t sliceLength);
		FileSliceReader Slice(uint64_t sliceStartPosition, uint64_t sliceLength) const;

		inline const std::string& GetFilename() const {
			return wrappedStream.GetFilename();
		}

	protected:
		void ReadImplementation(void* buffer, std::size_t size) override;

	private:
		void Initialize();

		WrappedStreamType wrappedStream;
		const uint64_t startingOffset;
		const uint64_t sliceLength;
	};
}
