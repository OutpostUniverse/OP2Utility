#pragma once

#include "StreamReader.h"

#include <cstddef>
#include <type_traits>
#include <vector>
#include <array>
#include <string>

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

	// std::string prefixed by the string's size. The type of integer representing the size must be provided.
	// Does not null terminate string unless null terminator is specifcally included in passed std::string. 
	template<typename SizeType>
	void Write(const std::string& string) {
		auto stringSize = string.size();
		if (stringSize > std::numeric_limits<SizeType>::max()) {
			throw std::runtime_error("String's size is too large to write in provided size field");
		}
		auto typedSize = static_cast<SizeType>(stringSize);
		Write(typedSize);
		Write(string.data(), string.size());
	}

	// Copy a StreamReader to a StreamWriter
	static const std::size_t DefaultCopyChunkSize = 0x00020000;
	template<std::size_t BufferSize = DefaultCopyChunkSize>
	void Write(StreamReader& streamReader) {
		std::array<char, BufferSize> buffer;
		std::size_t numBytesRead;

		do {
			// Read the input stream
			numBytesRead = streamReader.ReadPartial(buffer.data(), BufferSize);
			Write(buffer.data(), numBytesRead);
		} while (numBytesRead); // End loop when numBytesRead/Written is equal to 0
	}
};
