#pragma once

#include "ForwardSeekableReader.h"
#include <cstdint>

namespace Stream
{
	class BiDirectionalSeekableReader : public ForwardSeekableReader {
	public:
		// Seek backward by a relative amount, given as offset from current position
		virtual void SeekBackward(uint64_t offset) = 0;

		// Seek to absolute position, given as offset from beginning of stream
		virtual void Seek(uint64_t position) = 0;
	};
}
