#pragma once

#include <cstdint>

class StreamWriter {
public:
	virtual ~StreamWriter();
	virtual void Write(const void* buffer, size_t size) = 0;
};
