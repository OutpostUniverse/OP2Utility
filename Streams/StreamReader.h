#pragma once

#include <cstddef>

class StreamReader {
public:
	virtual ~StreamReader() = default;

	inline void Read(void* buffer, std::size_t size) {
		ReadImplementation(buffer, size);
	}

	// Inline templated convenience methods, to easily read arbitrary data types
	template<typename T> inline void Read(T& object) {
		ReadImplementation(&object, sizeof(object));
	}

protected:
	// When creating child classes, override this function to implement new read functionality
	// This function is required because to allow the base class to contain the Read template function.
	virtual void ReadImplementation(void* buffer, std::size_t size) = 0;
};
