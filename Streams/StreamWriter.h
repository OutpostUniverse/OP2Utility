#pragma once

#include <cstddef>

class StreamWriter {
public:
	virtual ~StreamWriter() = default;
	virtual void Write(const void* buffer, std::size_t size) = 0;
};
