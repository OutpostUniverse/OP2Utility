#pragma once

#include <cstddef>
#include <type_traits>

class StreamWriter {
public:
	virtual ~StreamWriter() = default;

	inline void Write(const void* buffer, std::size_t size) {
		WriteImplementation(buffer, size);
	}

	// Inline templated convenience methods, to easily write arbitrary data types
	template<typename T>
	inline std::enable_if_t<std::is_trivially_copyable<T>::value> Write(const T& object) {
		WriteImplementation(&object, sizeof(object));
	}

protected:
	// WriteImplementation is named differently from Write to prevent name hiding of the 
	// Write template helpers in derived classes.
	virtual void WriteImplementation(const void* buffer, std::size_t size) = 0;
};
