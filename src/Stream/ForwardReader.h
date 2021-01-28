#pragma once

#include "Reader.h"
#include <cstdint>

namespace OP2Utility::Stream
{
	class ForwardReader : public Reader {
	public:
		// Get the size of the stream
		virtual uint64_t Length() = 0;

		// Get the current position of the stream
		virtual uint64_t Position() = 0;

		// Seek forward by a relative amount, given as offset from current position
		virtual void SeekForward(uint64_t offset) = 0;

		void SeekEnd() {
			SeekForward(Length() - Position());
		}
	};
}
