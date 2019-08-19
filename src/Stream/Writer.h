#pragma once

#include "Reader.h"

#include <cstddef>
#include <type_traits>
#include <limits>
#include <vector>
#include <array>
#include <string>
#include <stdexcept>

namespace Stream
{
	class Writer {
	protected:
		// Generic write method, which raises an exception if the full data can not be written
		// Note: This is named separately from Write to prevent name hiding in derived classes
		// All Write methods are syntax sugar which delegate to this method
		virtual void WriteImplementation(const void* buffer, std::size_t size) = 0;

	public:
		virtual ~Writer() = default;

		inline void Write(const void* buffer, std::size_t size) {
			WriteImplementation(buffer, size);
		}

		// Inline templated convenience methods to easily write more complex types
		// ====

		// Trivially copyable data types
		template<typename T>
		inline
		std::enable_if_t<std::is_trivially_copyable<T>::value>
		Write(const T& object) {
			WriteImplementation(&object, sizeof(object));
		}

		// Non-trivial contiguous container of trivially copyable data types
		template<typename T>
		inline
		std::enable_if_t<
			!std::is_trivially_copyable<T>::value &&
			std::is_trivially_copyable<typename T::value_type>::value
		>
		Write(const T& container) {
			// Size calculation can't possibly overflow since the container size necessarily fits in memory
			WriteImplementation(container.data(), container.size() * sizeof(typename T::value_type));
		}

		// Size prefixed container data types
		// Ex: Write<uint32_t>(vector); // Write 32-bit vector size followed by vector data
		template<typename SizeType, typename T>
		std::enable_if_t<
			true
		>
		Write(const T& container) {
			auto containerSize = container.size();
			// This check is trivially false if SizeType is larger than max container size
			if constexpr(std::numeric_limits<decltype(containerSize)>::max() > std::numeric_limits<SizeType>::max()) {
				if (containerSize > std::numeric_limits<SizeType>::max()) {
					throw std::runtime_error("Container too large to save size field");
				}
			}
			// Cast can't overflow due to check above
			Write(static_cast<SizeType>(containerSize));
			Write(container);
		}

		// Copy a Reader to a Writer
		static const std::size_t DefaultCopyChunkSize = 0x00020000;
		template<std::size_t BufferSize = DefaultCopyChunkSize>
		void Write(Reader& streamReader) {
			std::array<char, BufferSize> buffer;
			std::size_t numBytesRead;

			do {
				// Read the input stream
				numBytesRead = streamReader.ReadPartial(buffer.data(), BufferSize);
				Write(buffer.data(), numBytesRead);
			} while (numBytesRead); // End loop when numBytesRead/Written is equal to 0
		}
	};
}
