#pragma once

#include "FileStreamReader.h"
#include <string>
#include <cstddef>
#include <cstdint>

// Opens a new file stream that is limited to reading from the provided file slice. 
class FileSliceReader : public SeekableStreamReader
{
public:
	FileSliceReader(std::string filename, uint64_t startingOffset, uint64_t sliceLength);

	// SeekableStreamReader methods
	uint64_t Length() override;
	uint64_t Position() override;
	void Seek(uint64_t position) override;
	void SeekRelative(int64_t offset) override;

	std::unique_ptr<SeekableStreamReader> Slice(uint64_t sliceLength);
	std::unique_ptr<SeekableStreamReader> Slice(uint64_t sliceStartPosition, uint64_t sliceLength) const;

	inline const std::string& GetFilename() const {
		return fileStreamReader.GetFilename();
	}

protected:
	void ReadImplementation(void* buffer, std::size_t size) override;

private:
	FileStreamReader fileStreamReader;
	const uint64_t startingOffset;
	const uint64_t sliceLength;
};
