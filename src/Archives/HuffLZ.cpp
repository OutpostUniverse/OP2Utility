#include <string.h>
#include "HuffLZ.h"

namespace Archives
{
	// Constructs the object around an existing bit stream
	HuffLZ::HuffLZ(BitStream *bitStream)
	{
		m_ContructedBitStream = 0;				// Don't need to delete stream in destructor
		m_BitStream = bitStream;				// Store a reference to the bit stream
		m_HuffTree = new AdaptHuffTree(314);	// Construct the Adaptive Huffman tree
		m_BuffWriteIndex = 0;
		m_BuffReadIndex = 0;
		m_EOS = false;

		// Initialize the decompress buffer to spaces
		memset(m_DecompressBuffer, ' ', 4096);
	}

	// Creates an internal bit stream for the buffer
	HuffLZ::HuffLZ(std::size_t bufferSize, void *buffer)
	{
		// Construct the BitStream object
		m_BitStream = new BitStream(bufferSize, buffer);
		m_ContructedBitStream = m_BitStream;	// Remeber to delete this in the destructor

		m_HuffTree = new AdaptHuffTree(314);	// Construct the Adaptive Huffman tree
		m_BuffWriteIndex = 0;
		m_BuffReadIndex = 0;
		m_EOS = false;

		// Initialize the decompress buffer to spaces
		memset(m_DecompressBuffer, ' ', 4096);
	}

	HuffLZ::~HuffLZ()
	{
		delete m_ContructedBitStream;
		delete m_HuffTree;
	}



	// Copies up to bufferSize bytes into buffer from the decoded data. If not enough
	// data is available to fill the buffer, more of the input is decompressed up to
	// the end of the input stream or until the buffer is filled. Returns the total
	// number of bytes that were copied.
	std::size_t HuffLZ::GetData(std::size_t bufferSize, char *buffer)
	{
		std::size_t numBytesCopied;
		std::size_t numBytesTotal = 0;

		// Try to keep the buffer largely full
		// This allows allows more memory to be copied at once
		FillDecompressBuffer();

		// Copy what is currently available
		numBytesCopied = CopyAvailableData(bufferSize, buffer);
		numBytesTotal += numBytesCopied;
		bufferSize -= numBytesCopied;

		// Decompress further to fill buffer or till end of stream
		while (bufferSize && !m_EOS)
		{
			FillDecompressBuffer();		// Decompress a bunch
			numBytesCopied = CopyAvailableData(bufferSize, &buffer[numBytesTotal]);
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
		if (m_BuffWriteIndex < m_BuffReadIndex)
			*sizeAvailableData = 4096 - m_BuffReadIndex;
		else
			*sizeAvailableData = (m_BuffWriteIndex - m_BuffReadIndex);
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
		if (m_EOS) return;

		// Try to fill the buffer (largely anyways)
		while (((m_BuffWriteIndex - m_BuffReadIndex) & 0x0FFF) < maxFill)
		{
			m_EOS = DecompressCode();		// Decompress another code
			if (m_EOS) break;				// Break on end of stream
		}
	}

	// Copies all available data into buff up to a maximum of size bytes
	// This routine handles the case when the copy must wrap around the circular buffer
	std::size_t HuffLZ::CopyAvailableData(std::size_t size, char *buff)
	{
		std::size_t numBytesToCopy;
		std::size_t numBytesTotal = 0;

		// Check if buffer is empty
		// Note: This isn't really needed but should speed things up
		if (m_BuffWriteIndex == m_BuffReadIndex)
			return 0;

		// Check if wrap around is required to copy (all available) data
		if (m_BuffWriteIndex < m_BuffReadIndex)
		{
			// Must wrap around to copy (all available) data
			numBytesToCopy = 4096 - m_BuffReadIndex;
			// Make sure not to overflow the output buffer
			if (numBytesToCopy > size) numBytesToCopy = size;
			// Copy what is already decompressed into the output buffer
			memcpy(buff, &m_DecompressBuffer[m_BuffReadIndex], numBytesToCopy);
			numBytesTotal = numBytesToCopy;	// Update number of bytes copied
			size -= numBytesToCopy;			// Update space left in buffer
			// Update read index and wrap around 4096
			m_BuffReadIndex = (m_BuffReadIndex + numBytesToCopy) & 0x0FFF;
		}

		// Determine how much data is currently available (Note: No need to wrap around 4096)
		numBytesToCopy = (m_BuffWriteIndex - m_BuffReadIndex);
		// Make sure not to overflow the output buffer
		if (numBytesToCopy > size) numBytesToCopy = size;
		if (numBytesToCopy > 0)
		{
			// Copy what is already decompressed into the output buffer
			memcpy(&buff[numBytesTotal], &m_DecompressBuffer[m_BuffReadIndex], numBytesToCopy);
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
		int start;
		int offset;

		// Get the next code
		code = GetNextCode();
		// Update the tree
		m_HuffTree->UpdateCodeCount(code);

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
		return m_BitStream->EndOfStream();
	}


	// Performs a tree lookup guided by the bitstream to find the next code
	int HuffLZ::GetNextCode()
	{
		int nodeIndex;
		bool bBit;

		// Use bitstream to find a terminal node
		nodeIndex = m_HuffTree->GetRootNodeIndex();
		while (!m_HuffTree->IsLeaf(nodeIndex))
		{
			bBit = m_BitStream->ReadNextBit();
			nodeIndex = m_HuffTree->GetChildNode(nodeIndex, bBit);
		}

		return m_HuffTree->GetNodeData(nodeIndex);
	}

	// Determines the offset to the start of a repeated block. (This one is a little weird)
	int HuffLZ::GetRepeatOffset()
	{
		int numExtraBits;
		int mod;
		int offset;

		// Get the next 8 bits
		offset = m_BitStream->ReadNext8Bits();

		// Determine how many more bits to read in
		if (offset < 0x20) numExtraBits = 1;
		else if (offset < 0x50) numExtraBits = 2;
		else if (offset < 0x90) numExtraBits = 3;
		else if (offset < 0xC0) numExtraBits = 4;
		else if (offset < 0xF0) numExtraBits = 5;
		else numExtraBits = 6;

		// Determine how to modify bits to get real offset
		if (offset < 0x20) mod = 0;
		else if (offset < 0x50) mod = ((offset - 0x20) >> 4) + 1;
		else if (offset < 0x90) mod = ((offset - 0x50) >> 3) + 4;
		else if (offset < 0xC0) mod = ((offset - 0x90) >> 2) + 0x0C;
		else if (offset < 0xF0) mod = ((offset - 0xC0) >> 1) + 0x18;
		else mod = (offset - 0xC0);

		// Read in the extra bits
		for (; numExtraBits; numExtraBits--) {
			offset = (offset << 1) + m_BitStream->ReadNextBit();
		}
		offset &= 0x3F;			// Mask upper bits (keep lower 6)

		// Apply the modifier
		offset |= (mod << 6);	// Set upper 6 bits (12 bit number -> buffer size is 4096)

		return offset;			// Return the offset to the start of the repeated block
	}

	void HuffLZ::WriteCharToBuffer(char c)
	{
		m_DecompressBuffer[m_BuffWriteIndex] = c;			// Write the char to the buffer
		m_BuffWriteIndex = (m_BuffWriteIndex + 1) & 0x0FFF;	// Wrap around 4096
	}
}
