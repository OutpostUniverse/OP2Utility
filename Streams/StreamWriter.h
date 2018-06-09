#pragma once

#include <cstdint>

class StreamWriter {
public:
	virtual ~StreamWriter() = 0 { };
	virtual void Write(const void* buffer, size_t size) = 0;
};
