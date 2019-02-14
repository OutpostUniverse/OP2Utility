#pragma once

#include "BidirectionalReader.h"
#include <string>
#include <fstream>
#include <cstddef>
#include <cstdint>

namespace Stream
{
	class FileReader;
	template <class WrappedStreamType> class SliceReader;
	using FileSliceReader = SliceReader<FileReader>;

	class FileReader : public BidirectionalReader {
	public:
		FileReader(std::string filename);
		FileReader(const FileReader& fileStreamReader);
		~FileReader() override;

		std::size_t ReadPartial(void* buffer, std::size_t size) noexcept override;

		// BidirectionalReader methods
		uint64_t Length() override;
		uint64_t Position() override;

		void Seek(uint64_t position) override;
		void SeekForward(uint64_t offset) override;
		void SeekBackward(uint64_t offset) override;

		// Create a slice of the stream for independent processing. Starts at current position of stream.
		// Seeks parent stream forward the slice's length if creation is successful.
		FileSliceReader Slice(uint64_t sliceLength);

		// Create a slice of the stream for independent processing.
		FileSliceReader Slice(uint64_t sliceStartPosition, uint64_t sliceLength) const;

		inline const std::string& GetFilename() const {
			return filename;
		}

	protected:
		void ReadImplementation(void* buffer, std::size_t size) override;

	private:
		void Initialize();

		const std::string filename;
		std::ifstream file;
	};
}
