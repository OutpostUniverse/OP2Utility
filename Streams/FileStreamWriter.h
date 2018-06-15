#pragma once

#include "SeekableStreamWriter.h"
#include <fstream>
#include <string>
#include <cstdint>

class FileStreamWriter : public SeekableStreamWriter
{
public:
	FileStreamWriter(const std::string& filename);

	// StreamWriter methods
	~FileStreamWriter() override;
	void Write(const void* buffer, std::size_t size) override;

	// Inline templated convenience methods, to easily read arbitrary data types
	template<typename T> inline void Write(const T& object) {
		Write(&object, sizeof(object));
	}

	// SeekableStreamWriter methods
	uint64_t Length() override;
	uint64_t Position() override;
	void Seek(uint64_t offset) override;
	void SeekRelative(int64_t offset) override;

private:
	std::fstream fileStream;
};
