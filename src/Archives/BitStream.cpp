#include "BitStream.h"
#include <stdexcept>
#include <string>

namespace Archives
{
	BitStream::BitStream() : 
		m_BufferBitSize(0),
		m_BufferSize(0),
		m_Buffer(0), 
		m_ReadBitIndex(0), 
		m_WriteBitIndex(0),
		m_WriteBuff(0) { }

	BitStream::BitStream(std::size_t bufferSize, void *buffer) : 
		m_BufferBitSize(bufferSize << 3),
		m_BufferSize(bufferSize),
		m_Buffer((unsigned char*)buffer),
		m_ReadBitIndex(0),
		m_WriteBitIndex(0),
		m_WriteBuff(0)
	{
		// Check if storing bufferSize in bits wraps past largest value of a std::size_t
		if (m_BufferBitSize < m_BufferSize) {
			throw std::runtime_error("BitStream cannot support a buffer size of " + std::to_string(bufferSize));
		}
	}




	bool BitStream::ReadNextBit()
	{
		bool bNextBit;

		// Check for end of stream
		if (m_ReadBitIndex >= m_BufferBitSize) {
			return 0;
		}
		
		// Check if a new byte needs to be buffered
		if ((m_ReadBitIndex & 0x07) == 0) {
			m_ReadBuff = m_Buffer[m_ReadBitIndex >> 3];
		}

		// Extract the uppermost bit
		bNextBit = (m_ReadBuff & 0x80) == 0x80;	// Get the MSB

		// Update the Read Buffer and Read Index
		m_ReadBuff <<= 1;	// Shift buffer left 1 bit (MSB out)
		m_ReadBitIndex++;

		return bNextBit;	// Return the next bit in the stream
	}

	int BitStream::ReadNext8Bits()
	{
		int value;

		// Check for end of stream
		if (m_ReadBitIndex >= m_BufferBitSize) {
			return 0;
		}

		int i = m_ReadBitIndex & 0x07;
		if (i == 0)
		{
			// Read the next byte and return it
			value = m_Buffer[m_ReadBitIndex >> 3];
			m_ReadBitIndex += 8;
			return value;
		}

		value = m_ReadBuff;
		m_ReadBitIndex += 8;
		
		// Check for end of stream
		if (m_ReadBitIndex >= m_BufferBitSize) {
			m_ReadBuff = 0;
		}
		else {
			m_ReadBuff = m_Buffer[m_ReadBitIndex >> 3];
		}

		value |= (m_ReadBuff >> (8 - i));
		m_ReadBuff <<= i;
		return value;
	}


	bool BitStream::EndOfStream() const
	{
		return m_ReadBitIndex >= m_BufferBitSize;
	}

	std::size_t BitStream::GetBitReadPos() const
	{
		return m_ReadBitIndex;
	}
}
