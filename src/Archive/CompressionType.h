#pragma once

#include <cstdint>

namespace Archive
{
	// Represents compression flags used by Outpost 2 .VOL files to compress files
	// See FileFormat Archive Vol.txt for more details
	enum class CompressionType : uint16_t
	{
		Uncompressed = 0x100,
		RLE = 0x101,  // Run Length Encoded. Currently not supported.
		LZ = 0x102,   // Lempel - Ziv, named after the two creators. Currently not supported.
		LZH = 0x103,  // Lempel - Ziv, with adaptive Huffman encoding.
	};
}
