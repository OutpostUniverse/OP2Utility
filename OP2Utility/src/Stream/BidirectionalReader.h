#pragma once

#include "ForwardReader.h"
#include <cstdint>

namespace OP2Utility::Stream
{
	class BidirectionalReader : public ForwardReader {
	public:
		~BidirectionalReader() override;

		void Peek(void* buffer, std::size_t size) {
			ReadImplementation(buffer, size);
			SeekBackward(size);
		}

		// Trivially copyable data types
		template<typename T>
		std::enable_if_t<std::is_trivially_copyable<T>::value>
		Peek(T& object) {
			Peek(&object, sizeof(object));
		}

		// Seek backward by a relative amount, given as offset from current position
		virtual void SeekBackward(uint64_t offset) = 0;

		// Seek to absolute position, given as offset from beginning of stream
		virtual void Seek(uint64_t position) = 0;

		void SeekBeginning() {
			Seek(0);
		}
	};
}
