#pragma once

#include "SeekableStreamReader.h"
#include <string>
#include <fstream>
#include <cstddef>
#include <cstdint>

class FileStreamReader : public SeekableStreamReader {
public:
	FileStreamReader(std::string fileName);
	~FileStreamReader() override;

	// SeekableStreamReader methods
	uint64_t Length() override;
	uint64_t Position() override;
	void Seek(uint64_t position) override;
	void SeekRelative(int64_t offset) override;

	inline const std::string& GetFilename() const {
		return filename;
	}

protected:
	void ReadImplementation(void* buffer, std::size_t size) override;

private:
	const std::string filename;
	std::ifstream file;
};
