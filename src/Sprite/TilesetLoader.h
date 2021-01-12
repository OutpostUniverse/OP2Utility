#pragma once

#include "../Tag.h"

class BitmapFile;

namespace Stream {
	class Reader;
	class BidirectionalReader;
	class Writer;
}


namespace Tileset
{
	constexpr auto TagFileSignature = MakeTag("PBMP");

	bool PeekIsCustomTileset(Stream::BidirectionalReader& reader);

	// Read either custom tileset format or standard bitmap format tileset into memory
	// After reading into memory, if needed, reformats into standard 8 bit indexed bitmap before returning
	BitmapFile ReadTileset(Stream::BidirectionalReader& reader);
	BitmapFile ReadTileset(Stream::BidirectionalReader&& reader);

	// Read tileset represented by Outpost 2 specific format into memory
	// After Reading into memory, reformats into standard 8 bit indexed bitmap before returning results
	BitmapFile ReadCustomTileset(Stream::Reader& reader);

	// Write tileset in Outpost 2's custom bitmap format.
	// To write tileset in standard bitmap format, use BitmapFile::WriteIndexed
	void WriteCustomTileset(Stream::Writer& writer, const BitmapFile& tileset);

	// Throw runtime error if provided bitmap does not meet specific tileset requirements
	// Assumes provided tileset is already properly formed to standard bitmap file format
	void ValidateTileset(const BitmapFile& tileset);
}
