#pragma once

#include <cstddef>

class StreamWriter {
public:
	virtual ~StreamWriter() = default;

	inline void Write(const void* buffer, std::size_t size) {
		WriteImplementation(buffer, size);
	}

	// Inline templated convenience methods, to easily write arbitrary data types
	template<typename T> inline void Write(const T& object) {
		WriteImplementation(&object, sizeof(object));
	}

protected:
	// When creating child classes, override this function to implement new write functionality
	// This function is required because to allow the base class to contain the Write template function.
	virtual void WriteImplementation(const void* buffer, std::size_t size) = 0;
};
