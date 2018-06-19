#pragma once

#include "FileStreamReader.h"
#include <cstddef>
#include <string>

class FileSliceReader : public SeekableStreamReader
{
public:
	FileSliceReader(std::string filename, uint64_t startingOffset, uint64_t sliceLength);

	uint64_t Length() override;
	uint64_t Position() override;
	void Seek(uint64_t position) override;
	void SeekRelative(int64_t offset) override;

protected:
	void ReadImplementation(void* buffer, std::size_t size) override;

private:
	FileStreamReader fileStreamReader;
	uint64_t startingOffset;
	uint64_t sliceLength;
};
