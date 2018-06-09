#pragma once

#include <cstdint>

class StreamReader {
public:
	virtual ~StreamReader() = 0 { };
	virtual void Read(void* buffer, size_t size) = 0;
};
