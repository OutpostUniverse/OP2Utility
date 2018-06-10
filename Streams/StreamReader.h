#pragma once

#include <cstdint>

class StreamReader {
public:
	virtual ~StreamReader();
	virtual void Read(void* buffer, size_t size) = 0;
};
