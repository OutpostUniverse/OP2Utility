#pragma once

#include <cstddef>

class StreamReader {
protected:
	// ReadImplementation is named differently from Read to prevent name hiding of the
	// Read template helpers in derived classes.
	virtual void ReadImplementation(void* buffer, std::size_t size) = 0;

public:
	virtual ~StreamReader() = default;

	// Helper methods, which depend only on the above interface
	// ====

	inline void Read(void* buffer, std::size_t size) {
		ReadImplementation(buffer, size);
	}

	// Inline templated convenience methods, to easily read arbitrary data types
	template<typename T> inline void Read(T& object) {
		ReadImplementation(&object, sizeof(object));
	}
};
