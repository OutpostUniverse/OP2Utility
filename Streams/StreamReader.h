#pragma once

#include <cstddef>
#include <type_traits>
#include <vector>

class StreamReader {
public:
	virtual ~StreamReader() = default;

	inline void Read(void* buffer, std::size_t size) {
		ReadImplementation(buffer, size);
	}

	// Inline templated convenience methods to easily read more complex types
	// ====

	// Trivially copyable data types
	template<typename T>
	inline std::enable_if_t<std::is_trivially_copyable<T>::value> Read(T& object) {
		ReadImplementation(&object, sizeof(object));
	}

	// Vector data types
	template<typename T, typename A>
	inline void Read(std::vector<T, A>& vector) {
		ReadImplementation(vector.data(), vector.size() * sizeof(T));
	}

protected:
	// ReadImplementation is named differently from Read to prevent name hiding of the 
	// Read template helpers in derived classes.
	virtual void ReadImplementation(void* buffer, std::size_t size) = 0;
};
