#pragma once

#include "SeekableReader.h"
#include <string>
#include <fstream>
#include <cstddef>
#include <cstdint>

namespace Stream
{
	class FileSliceReader;

	class FileReader : public SeekableReader {
	public:
		FileReader(std::string filename);
		FileReader(const FileReader& fileStreamReader);
		~FileReader() override;

		std::size_t ReadPartial(void* buffer, std::size_t size) noexcept override;

		// SeekableReader methods
		uint64_t Length() override;
		uint64_t Position() override;
		void Seek(uint64_t position) override;
		void SeekRelative(int64_t offset) override;

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