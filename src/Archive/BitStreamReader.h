#pragma once

#include <cstddef>

namespace Archive
{
	// This is designed for use in the .vol file decompressor
	class BitStreamReader
	{
	public:
		BitStreamReader(void *buffer, std::size_t bufferSize); // Construct stream around given buffer

		bool ReadNextBit();			// Get bit at Read index and advance index
		int  ReadNext8Bits();		// Get next 8 bits at Read index and advance index
		bool EndOfStream() const;	// Returns true if the last bit has been read
		std::size_t GetBitReadPos() const; // Returns the position (in bits) of the read pointer
	private:
		std::size_t m_BufferBitSize;// Size of buffer in bits
		unsigned char *m_Buffer;	// Byte array of data

		std::size_t m_ReadBitIndex;	// Current bit being read from the stream

		unsigned char m_ReadBuff;	// 1 Byte read buffer (shift bits out of)
	};
}
