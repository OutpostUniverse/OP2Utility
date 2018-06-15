#pragma once

#include <cstddef>

class StreamReader {
public:
	virtual ~StreamReader() = default;
	virtual void Read(void* buffer, std::size_t size) = 0;
};
