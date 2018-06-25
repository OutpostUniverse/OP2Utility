#pragma once

#include "SeekableStreamWriter.h"
#include <string>
#include <fstream>
#include <cstddef>
#include <cstdint>

class FileStreamWriter : public SeekableStreamWriter
{
public:
	FileStreamWriter(const std::string& filename);
	~FileStreamWriter() override;

	// SeekableStreamWriter methods
	uint64_t Length() override;
	uint64_t Position() override;
	void Seek(uint64_t offset) override;
	void SeekRelative(int64_t offset) override;

	inline const std::string& GetFilename() const {
		return filename;
	}

protected:
	void WriteImplementation(const void* buffer, std::size_t size) override;

private:
	const std::string filename;
	std::fstream fileStream;
};
