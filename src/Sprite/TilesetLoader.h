#pragma once

#include "../Tag.h"

class BitmapFile;

namespace Stream {
	class BidirectionalReader;
}


namespace TilesetLoader
{
	constexpr Tag TagFileSignature = MakeTag("PBMP");

	bool PeekIsCustomTileset(Stream::BidirectionalReader& reader);

	// Throw runtime error if provided bitmap does not meet specific tileset requirements
	// Assumes provided tileset is already properly formed to standard bitmap file format
	void ValidateTileset(const BitmapFile& tileset);
}
