#pragma once

#include <cstddef>

class StreamReader {
protected:
	// Generic read method, which raises an exception if the full data can not be read
	// Note: This is named separately from Read to prevent name hiding in derived classes
	// All Read methods are syntax sugar which delegate to this method
	virtual void ReadImplementation(void* buffer, std::size_t size) = 0;

public:
	// Generic read method, which returns the number of bytes transferred
	// This method is similar to Read, except it does not raise an exception if the buffer can not be filled
	virtual std::size_t ReadPartial(void* buffer, std::size_t size) noexcept = 0;

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
