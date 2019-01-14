// This decompressor is meant to be compatible with
// the compression in .vol files.

#include "AdaptiveHuffmanTree.h"
#include "BitStreamReader.h"
#include <cstddef>

namespace Archive
{
	class HuffLZ
	{
	public:
		HuffLZ(const BitStreamReader& bitStreamReader);

		std::size_t GetData(char *buffer, std::size_t bufferSize);	// Copy decoded data into given buffer.
													// Returns number of bytes copied
		const char* GetInternalBuffer(std::size_t *sizeAvailableData);
		// Give access to internal decompress
		// buffer (no memory copy required)
	private:
		void InitializeDecompressBuffer();
		void FillDecompressBuffer();				// Decompress until buffer is near full
		std::size_t CopyAvailableData(char *buff, std::size_t size);// Copies already decompressed data
		bool DecompressCode();	// Decompresses a code and returns false at end of stream
		int GetNextCode();
		unsigned int GetRepeatOffset();
		void WriteCharToBuffer(char c);

		struct OffsetModifiers {
			unsigned int extraBitCount;
			unsigned int offsetUpperBits;
		};
		static OffsetModifiers GetOffsetModifiers(unsigned int offset);

		// Member variables
		BitStreamReader m_BitStreamReader;
		AdaptiveHuffmanTree m_AdaptiveHuffmanTree;
		char m_DecompressBuffer[4096];				// Circular decompression buffer
		std::size_t m_BuffWriteIndex;
		std::size_t m_BuffReadIndex;
		bool m_EOS;									// End of Stream
	};
}
