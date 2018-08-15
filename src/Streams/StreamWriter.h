#pragma once

#include "StreamReader.h"

#include <cstddef>
#include <type_traits>
#include <limits>
#include <vector>
#include <array>
#include <string>
#include <stdexcept>

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

	// Size prefixed vector data types
	// Ex: Write<uint32_t>(vector); // Write 32-bit vector size followed by vector data
	template<typename SizeType, typename T, typename A>
	void Write(const std::vector<T, A>& vector) {
		auto vectorSize = vector.size();
		if (vectorSize > std::numeric_limits<SizeType>::max()) {
			throw std::runtime_error("Vector too large to save size field");
		}
		auto typedSize = static_cast<SizeType>(vectorSize);
		Write(typedSize);
		Write(vector);
	}

	// String data types
	// Does not write null terminator unless specifically included in string
	template<typename CharT, typename Traits, typename Allocator>
	void Write(const std::basic_string<CharT, Traits, Allocator>& string) {
		Write(&string[0], string.size() * sizeof(CharT));
	}

	// Size prefixed string data types
	// Ex: Write<uint32_t>(string); // Write 32-bit string length followed by string data
	// Does not write null terminator unless specifically included in string
	template<typename SizeType, typename CharT, typename Traits, typename Allocator>
	void Write(const std::basic_string<CharT, Traits, Allocator>& string) {
		auto stringSize = string.size();
		if (stringSize > std::numeric_limits<SizeType>::max()) {
			throw std::runtime_error("String's size is too large to write in provided size field");
		}
		Write(static_cast<SizeType>(stringSize));
		Write(string);
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
