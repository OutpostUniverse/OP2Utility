#pragma once

#include <cstddef>
#include <type_traits>
#include <vector>
#include <string>
#include <stdexcept>

namespace Stream
{
	class Reader {
	protected:
		// Generic read method, which raises an exception if the full data can not be read
		// Note: This is named separately from Read to prevent name hiding in derived classes
		// All Read methods are syntax sugar which delegate to this method
		virtual void ReadImplementation(void* buffer, std::size_t size) = 0;

	public:
		// Generic read method, which returns the number of bytes transferred
		// This method is similar to Read, except it does not raise an exception if the buffer can not be filled
		virtual std::size_t ReadPartial(void* buffer, std::size_t size) noexcept = 0;

		virtual ~Reader() = default;

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
		// Reads into entire length of passed vector. Call vector.resize(vectorSize) before 
		// passing vector into this function to ensure proper vector size is read
		template<typename T, typename A>
		inline void Read(std::vector<T, A>& vector) {
			ReadImplementation(vector.data(), vector.size() * sizeof(T));
		}

		// Size prefixed vector data types
		// Ex: Read<uint32_t>(vector); // Read 32-bit vector size, allocate space, then read vector data
		template<typename SizeType, typename T, typename A>
		void Read(std::vector<T, A>& vector) {
			SizeType vectorSize;
			Read(vectorSize);
			if (vectorSize < 0) {
				throw std::runtime_error("Vector's size may not be a negative number");
			}
			vector.clear();
			vector.resize(vectorSize);
			Read(vector);
		}

		// String data types
		// Reads into entire length of passed string. Call string.resize(stringSize) before 
		// passing string into this function to ensure proper string size is read
		template<typename CharT, typename Traits, typename Allocator>
		void Read(std::basic_string<CharT, Traits, Allocator>& string) {
			Read(&string[0], string.size() * sizeof(CharT));
		}

		// Size prefixed string data types
		// Ex: Read<uint32_t>(string); // Read 32-bit string length, allocate space, then read string data
		template<typename SizeType, typename CharT, typename Traits, typename Allocator>
		void Read(std::basic_string<CharT, Traits, Allocator>& string) {
			SizeType stringSize;
			Read(stringSize);
			if (stringSize < 0) {
				throw std::runtime_error("String's size may not be a negative number");
			}

			string.clear();
			string.resize(stringSize);
			Read(string);
		}
	};
}
