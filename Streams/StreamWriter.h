#pragma once

#include <cstdint>

class StreamWriter {
public:
	virtual ~StreamWriter() = default;
	virtual void Write(const void* buffer, size_t size) = 0;
};
