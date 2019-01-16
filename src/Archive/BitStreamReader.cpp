#include "BitStreamReader.h"
#include <stdexcept>
#include <string>
#include <limits>

namespace Archive
{
	BitStreamReader::BitStreamReader(void *buffer, std::size_t bufferSize) :
		m_BufferBitSize(bufferSize << 3),
		m_Buffer(static_cast<unsigned char*>(buffer)),
		m_ReadBitIndex(0),
		m_ReadBuff(0)
	{
		// Check bufferSize does not exceed the max addressable bit index
		if (bufferSize > std::numeric_limits<decltype(m_BufferBitSize)>::max() / 8) {
			throw std::runtime_error("BitStreamReader cannot support a buffer size of " + std::to_string(bufferSize));
		}
	}




	bool BitStreamReader::ReadNextBit()
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

	int BitStreamReader::ReadNext8Bits()
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


	bool BitStreamReader::EndOfStream() const
	{
		return m_ReadBitIndex >= m_BufferBitSize;
	}

	std::size_t BitStreamReader::GetBitReadPos() const
	{
		return m_ReadBitIndex;
	}
}
