#pragma once

#include "../Tag.h"

class BitmapFile;

namespace Stream {
	class Reader;
	class BidirectionalReader;
}


namespace Tileset
{
	constexpr auto TagFileSignature = MakeTag("PBMP");

	bool PeekIsCustomTileset(Stream::BidirectionalReader& reader);

	// Read tileset represented by Outpost 2 specific format into memory
	// After Reading into memory, reformats into standard 8 bit indexed bitmap before returning results
	BitmapFile ReadCustomTileset(Stream::Reader& reader);

	// Throw runtime error if provided bitmap does not meet specific tileset requirements
	// Assumes provided tileset is already properly formed to standard bitmap file format
	void ValidateTileset(const BitmapFile& tileset);
}
