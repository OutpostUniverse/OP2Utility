#pragma once

#include <cstddef>
#include <type_traits>
#include <vector>
#include <string>

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

	// std::string prefixed by the string's size. The type of integer representing the size must be provided. 
	template<typename SizeType>
	void Read(std::string& string) {
		SizeType stringSize;
		Read(stringSize);
		if (stringSize < 0) {
			throw std::runtime_error("String's size may not be a negative number");
		}

		string.clear();
		string.resize(stringSize);
		Read(&string[0], stringSize);
	}
};
