#pragma once

#include <cstddef>

class StreamWriter {
protected:
	// Generic write method, which raises an exception if the full data can not be written
	// Note: This is named separately from Write to prevent name hiding in derived classes
	// All Write methods are syntax sugar which delegate to this method
	virtual void WriteImplementation(const void* buffer, std::size_t size) = 0;

public:
	virtual ~StreamWriter() = default;

	// Helper methods, which depend only on the above interface
	// ====

	inline void Write(const void* buffer, std::size_t size) {
		WriteImplementation(buffer, size);
	}

	// Inline templated convenience methods, to easily write arbitrary data types
	template<typename T> inline void Write(const T& object) {
		WriteImplementation(&object, sizeof(object));
	}
};
