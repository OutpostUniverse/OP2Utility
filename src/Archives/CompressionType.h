#pragma once

namespace Archives
{
	// Represents compression flags used by Outpost 2 .VOL files to compress files
	// See FileFormat Archive Vol.txt for more details
	enum class CompressionType : unsigned short
	{
		Uncompressed = 0x100,
		RLE = 0x101,  // Run Length Encoded. Currently not supported.
		LZ = 0x102,   // Lempel - Ziv, named after the two creators. Currently not supported.
		LZH = 0x103,  // Lempel - Ziv, with adaptive Huffman encoding.
	};
}
