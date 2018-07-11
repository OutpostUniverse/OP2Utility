#pragma once

#include <cstddef>
#include <type_traits>

class StreamReader {
public:
	virtual ~StreamReader() = default;

	inline void Read(void* buffer, std::size_t size) {
		ReadImplementation(buffer, size);
	}

	// Inline templated convenience methods, to easily read arbitrary data types
	template<typename T>
	inline std::enable_if_t<std::is_trivially_copyable<T>::value> Read(T& object) {
		ReadImplementation(&object, sizeof(object));
	}

protected:
	// ReadImplementation is named differently from Read to prevent name hiding of the 
	// Read template helpers in derived classes.
	virtual void ReadImplementation(void* buffer, std::size_t size) = 0;
};
