#pragma once

#include "FileReader.h"
#include <string>
#include <cstddef>
#include <cstdint>

namespace Stream
{
	// Opens a new file stream that is limited to reading from the provided file slice.
	class FileSliceReader : public SeekableReader
	{
	public:
		FileSliceReader(std::string filename, uint64_t startingOffset, uint64_t sliceLength);
		FileSliceReader(const FileSliceReader& fileSliceReader);

		std::size_t ReadPartial(void* buffer, std::size_t size) noexcept override;

		// SeekableReader methods
		uint64_t Length() override;
		uint64_t Position() override;
		void Seek(uint64_t position) override;
		void SeekRelative(int64_t offset) override;

		FileSliceReader Slice(uint64_t sliceLength);
		FileSliceReader Slice(uint64_t sliceStartPosition, uint64_t sliceLength) const;

		inline const std::string& GetFilename() const {
			return fileStreamReader.GetFilename();
		}

	protected:
		void ReadImplementation(void* buffer, std::size_t size) override;

	private:
		void Initialize();

		FileReader fileStreamReader;
		const uint64_t startingOffset;
		const uint64_t sliceLength;
	};
}
