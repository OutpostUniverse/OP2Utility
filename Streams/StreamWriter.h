#pragma once

#include <cstddef>
#include <type_traits>
#include <vector>

class StreamWriter {
public:
	virtual ~StreamWriter() = default;

	inline void Write(const void* buffer, std::size_t size) {
		WriteImplementation(buffer, size);
	}

	// Inline templated convenience methods to easily write more complex types
	// ====

	// Trivially copyable data types
	template<typename T>
	inline std::enable_if_t<std::is_trivially_copyable<T>::value> Write(const T& object) {
		WriteImplementation(&object, sizeof(object));
	}

	// Vector data types
	template<typename T, typename A>
	inline void Write(const std::vector<T, A>& vector) {
		WriteImplementation(vector.data(), vector.size() * sizeof(T));
	}

protected:
	// WriteImplementation is named differently from Write to prevent name hiding of the 
	// Write template helpers in derived classes.
	virtual void WriteImplementation(const void* buffer, std::size_t size) = 0;
};
