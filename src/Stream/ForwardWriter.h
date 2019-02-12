#pragma once

#include "Writer.h"
#include <cstdint>

namespace Stream
{
	class ForwardWriter : public Writer
	{
	public:
		// Get the size of the stream
		virtual uint64_t Length() = 0;

		// Get the current position of the stream
		virtual uint64_t Position() = 0;

		// Seek forward by a relative amount, given as offset from current position
		virtual void SeekForward(uint64_t offset) = 0;
	};
}
