#include "HuffLZ.h"
#include <cstring>

namespace Archive
{
	// Constructs the object around an existing bit stream
	HuffLZ::HuffLZ(const BitStreamReader& bitStream) :
		m_BitStreamReader(bitStream),
		m_AdaptiveHuffmanTree(AdaptiveHuffmanTree(314)),
		m_BuffWriteIndex(0),
		m_BuffReadIndex(0),
		m_EOS(false)
	{
		InitializeDecompressBuffer();
	}

	void HuffLZ::InitializeDecompressBuffer() {
		// Initialize the decompress buffer to spaces
		std::memset(m_DecompressBuffer, ' ', 4096);
	}


	// Copies up to bufferSize bytes into buffer from the decoded data. If not enough
	// data is available to fill the buffer, more of the input is decompressed up to
	// the end of the input stream or until the buffer is filled. Returns the total
	// number of bytes that were copied.
	std::size_t HuffLZ::GetData(char *buffer, std::size_t bufferSize)
	{
		std::size_t numBytesCopied;
		std::size_t numBytesTotal = 0;

		// Try to keep the buffer largely full
		// This allows allows more memory to be copied at once
		FillDecompressBuffer();

		// Copy what is currently available
		numBytesCopied = CopyAvailableData(buffer, bufferSize);
		numBytesTotal += numBytesCopied;
		bufferSize -= numBytesCopied;

		// Decompress further to fill buffer or till end of stream
		while (bufferSize && !m_EOS)
		{
			FillDecompressBuffer();		// Decompress a bunch
			numBytesCopied = CopyAvailableData(&buffer[numBytesTotal], bufferSize);
			numBytesTotal += numBytesCopied;
			bufferSize -= numBytesCopied;
		}


		return numBytesTotal;
	}

	// Returns a pointer to the internal buffer at the current read index and
	// updates the current read index. The amount of available data is also returned.
	// Note: If the buffer wraps around, the remaining data before the wrap around
	//  is returned. A subsequent call will get the data after the wrap around.
	// Note: If sizeAvailableData == 0 then the end of the stream has been reached
	const char* HuffLZ::GetInternalBuffer(std::size_t *sizeAvailableData)
	{
		const char* currentPos;

		// Keep the buffer as full as possible (without danger of overflow)
		FillDecompressBuffer();

		// Determine how much data is available (without a wrap around)
		if (m_BuffWriteIndex < m_BuffReadIndex) {
			*sizeAvailableData = 4096 - m_BuffReadIndex;
		}
		else {
			*sizeAvailableData = (m_BuffWriteIndex - m_BuffReadIndex);
		}

		// Get the current read pointer
		currentPos = &m_DecompressBuffer[m_BuffReadIndex];
		// Update the read pointer
		m_BuffReadIndex = (m_BuffReadIndex + *sizeAvailableData) & 0x0FFF;

		// Return a pointer to the data
		return currentPos;
	}

	void HuffLZ::FillDecompressBuffer()
	{
		const int maxFill = 4096 - (314 - 253) - 1;
		// Decompress until the buffer is nearly full
		// Note: The longest a run can be is (314-1) - 253 so this is
		//  guaranteed not to overflow the buffer

		// Only try to decompress if not at end of stream
		if (m_EOS) {
			return;
		}

		// Try to fill the buffer (largely anyways)
		while (((m_BuffWriteIndex - m_BuffReadIndex) & 0x0FFF) < maxFill)
		{
			m_EOS = DecompressCode();		// Decompress another code
			if (m_EOS) {
				break;
			}
		}
	}

	// Copies all available data into buff up to a maximum of size bytes
	// This routine handles the case when the copy must wrap around the circular buffer
	std::size_t HuffLZ::CopyAvailableData(char *buff, std::size_t size)
	{
		std::size_t numBytesToCopy;
		std::size_t numBytesTotal = 0;

		// Check if buffer is empty
		// Note: This isn't really needed but should speed things up
		if (m_BuffWriteIndex == m_BuffReadIndex) {
			return 0;
		}

		// Check if wrap around is required to copy (all available) data
		if (m_BuffWriteIndex < m_BuffReadIndex)
		{
			// Must wrap around to copy (all available) data
			numBytesToCopy = 4096 - m_BuffReadIndex;

			// Make sure not to overflow the output buffer
			if (numBytesToCopy > size) {
				numBytesToCopy = size;
			}

			// Copy what is already decompressed into the output buffer
			std::memcpy(buff, &m_DecompressBuffer[m_BuffReadIndex], numBytesToCopy);
			numBytesTotal = numBytesToCopy;	// Update number of bytes copied
			size -= numBytesToCopy;			// Update space left in buffer
			// Update read index and wrap around 4096
			m_BuffReadIndex = (m_BuffReadIndex + numBytesToCopy) & 0x0FFF;
		}

		// Determine how much data is currently available (Note: No need to wrap around 4096)
		numBytesToCopy = (m_BuffWriteIndex - m_BuffReadIndex);

		// Make sure not to overflow the output buffer
		if (numBytesToCopy > size) {
			numBytesToCopy = size;
		}

		if (numBytesToCopy > 0)
		{
			// Copy what is already decompressed into the output buffer
			std::memcpy(&buff[numBytesTotal], &m_DecompressBuffer[m_BuffReadIndex], numBytesToCopy);
			numBytesTotal += numBytesToCopy;// Update number of bytes copied
			// Update read index (Note: no need to wrap around 4096 here)
			m_BuffReadIndex = (m_BuffReadIndex + numBytesToCopy);
		}

		return numBytesTotal;
	}


	// Performs one tree lookup and decompresses a portion based on the returned code.
	// Returns true if the end of the stream has been reached
	bool HuffLZ::DecompressCode()
	{
		unsigned short code;
		unsigned int start;
		unsigned int offset;

		// Get the next code
		code = GetNextCode();
		// Update the tree
		m_AdaptiveHuffmanTree.UpdateCodeCount(code);

		// Determine if the code is an ASCII code or a repeat block code
		if (code < 256)
		{
			// code is an ASCII code. Output it.
			WriteCharToBuffer((char)code);
		}
		else
		{
			// code is a repeat block code. Length of block is (code - 253)
			// Note: This implies the repeated blocks must be at least 3 characters long

			offset = GetRepeatOffset();		// Get the offset to the repeated block
			// Get start address of repeated block (wrap around 4096)
			start = (m_BuffWriteIndex - offset - 1) & 0x0FFF;

			// Copy for length = (code - 253) bytes
			for (code -= 253; code; code--, start = (start + 1) & 0x0FFF) {
				WriteCharToBuffer(m_DecompressBuffer[start]);
			}
		}

		// Check for the end of the stream
		return m_BitStreamReader.EndOfStream();
	}


	// Performs a tree lookup guided by the bitstream to find the next code
	int HuffLZ::GetNextCode()
	{
		int nodeIndex;
		bool bBit;

		// Use bitstream to find a terminal node
		nodeIndex = m_AdaptiveHuffmanTree.GetRootNodeIndex();
		while (!m_AdaptiveHuffmanTree.IsLeaf(nodeIndex))
		{
			bBit = m_BitStreamReader.ReadNextBit();
			nodeIndex = m_AdaptiveHuffmanTree.GetChildNode(nodeIndex, bBit);
		}

		return m_AdaptiveHuffmanTree.GetNodeData(nodeIndex);
	}

	// Determines the offset to the start of a repeated block
	//
	// Reads a variable length code from the bit stream (9-14 bits)
	// Smaller offsets have a shorter bit code
	// Returns a 12-bit offset (0..4095)
	unsigned int HuffLZ::GetRepeatOffset()
	{
		// Get the next 8 bits
		unsigned int offset = m_BitStreamReader.ReadNext8Bits();
		auto modifiers = GetOffsetModifiers(offset);

		// Read in the extra bits
		for (auto i = modifiers.extraBitCount; i; --i) {
			offset = (offset << 1) + m_BitStreamReader.ReadNextBit();
		}

		// Set upper 6 bits and preserve lower 6 bits (0x3F = 0011 1111)
		// Result is a 12-bit number with range 0..4095
		offset = (modifiers.offsetUpperBits << 6) | (offset & 0x3F);

		return offset;
	}

	void HuffLZ::WriteCharToBuffer(char c)
	{
		m_DecompressBuffer[m_BuffWriteIndex] = c;			// Write the char to the buffer
		m_BuffWriteIndex = (m_BuffWriteIndex + 1) & 0x0FFF;	// Wrap around 4096
	}



	HuffLZ::OffsetModifiers HuffLZ::GetOffsetModifiers(unsigned int offset) {
		if (offset < 0x20) {
			return { 1, 0 };
		}
		if (offset < 0x50) {
			return { 2, ((offset - 0x20) >> 4) + 1 };
		}
		if (offset < 0x90) {
			return { 3, ((offset - 0x50) >> 3) + 4 };
		}
		if (offset < 0xC0) {
			return { 4, ((offset - 0x90) >> 2) + 0x0C };
		}
		if (offset < 0xF0) {
			return { 5, ((offset - 0xC0) >> 1) + 0x18 };
		}

		return { 6, offset - 0xC0 };
	}
}
