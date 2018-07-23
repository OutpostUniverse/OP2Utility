#pragma once

#include <cstddef>
#include <type_traits>
#include <vector>

class StreamReader;


class StreamWriter {
protected:
	// Generic write method, which raises an exception if the full data can not be written
	// Note: This is named separately from Write to prevent name hiding in derived classes
	// All Write methods are syntax sugar which delegate to this method
	virtual void WriteImplementation(const void* buffer, std::size_t size) = 0;

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

	// Copy a StreamReader to a StreamWriter
	static const std::size_t DefaultCopyChunkSize = 0x00020000;
	template<std::size_t BufferSize = DefaultCopyChunkSize>
	void Write(StreamReader& streamReader);
};
