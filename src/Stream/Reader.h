#pragma once

#include <cstddef>
#include <type_traits>
#include <limits>
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
		inline
		std::enable_if_t<std::is_trivially_copyable<T>::value>
		Read(T& object) {
			ReadImplementation(&object, sizeof(object));
		}

		// Non-trivial contiguous container of trivially copyable data types
		// Reads into entire length of passed container. Call container.resize(size) before
		// passing container into this function to ensure proper container size is read
		template<typename T>
		inline
		std::enable_if_t<
			!std::is_trivially_copyable<T>::value &&
			std::is_trivially_copyable<typename T::value_type>::value
		>
		Read(T& container) {
			// Size calculation can't possibly overflow since the container size necessarily fits in memory
			ReadImplementation(container.data(), container.size() * sizeof(typename T::value_type));
		}

		// String data types
		// Reads into entire length of passed string. Call string.resize(stringSize) before
		// passing string into this function to ensure proper string size is read
		// Note: C++17 added a non-const overload of `string.data()`, needed to collapse this with the above
		template<typename CharT, typename Traits, typename Allocator>
		void Read(std::basic_string<CharT, Traits, Allocator>& string) {
			// Size calculation can't possibly overflow since the string size necessarily fits in memory
			Read(&string[0], string.size() * sizeof(CharT));
		}

		// Size prefixed container data types
		// Ex: Read<uint32_t>(vector); // Read 32-bit vector size, allocate space, then read vector data
		template<typename SizeType, typename T>
		void Read(T& container) {
			SizeType containerSize;
			Read(containerSize);
			// This check is trivially false for unsigned SizeType
			if constexpr(std::is_signed<SizeType>::value) {
				if (containerSize < 0) {
					throw std::runtime_error("Container's size may not be a negative number");
				}
			}
			// This check may be trivially false when SizeType is much smaller than max container size
			if (containerSize > container.max_size()) {
				throw std::runtime_error("Container's size is too big to fit in memory");
			}
			container.clear();
			container.resize(containerSize);
			Read(container);
		}

		// Read characters into a string until a null terminator is encountered
		// Does not include the null terminator in the returned string
		// Use maxCount to restrict the size of the returned string
		std::string ReadNullTerminatedString(std::size_t maxCount = SIZE_MAX)
		{
			std::string str;

			char c;
			for (std::size_t i = 0; i < maxCount; ++i)
			{
				Read(c);
				if (c == '\0') {
					break;
				}

				str.push_back(c);
			}

			return str;
		}
	};
}
