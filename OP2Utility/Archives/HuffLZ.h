// This decompressor is meant to be compatible with 
// the compression in .vol files.

#include "AdaptHuffTree.h"
#include "BitStream.h"

namespace Archives
{
	class HuffLZ
	{
	public:
		HuffLZ(BitStream *bitStream);
		HuffLZ(int bufferSize, char *buffer);
		~HuffLZ();

		int GetData(int bufferSize, char *buffer);	// Copy decoded data into given buffer.
													// Returns number of bytes copied
		const char* GetInternalBuffer(int *sizeAvailableData);
		// Give access to internal decompress
		// buffer (no memory copy required)
	private:
		void FillDecompressBuffer();				// Decompress until buffer is near full
		int CopyAvailableData(int size, char *buff);// Copies already decompressed data
		bool DecompressCode();	// Decompresses a code and returns false at end of stream
		int GetNextCode();
		int GetRepeatOffset();
		void WriteCharToBuffer(char c);

		// Member variables
		BitStream *m_BitStream;
		BitStream *m_ContructedBitStream;
		AdaptHuffTree *m_HuffTree;
		char m_DecompressBuffer[4096];				// Circular decompression buffer
		int m_BuffWriteIndex;
		int m_BuffReadIndex;
		bool m_EOS;									// End of Stream
	};
}
