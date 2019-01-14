#pragma once

#include <string>
#include <cstdint>

#pragma pack(push, 1) // Make sure structures are byte aligned

// Facilitates finding the source BMP file (well00XX.bmp) for a tile set.
// Tile set names must be exactly 8 chars long not including file extension.
struct TilesetSource
{
	// Fixed length string. Map and save files require tile set filenames to be exactly 8
	// characters long. User must handle terminating the string.
	std::string tilesetFilename;

	// Number of Tiles in set (represented on BMP).
	uint32_t numTiles;


	bool operator==(const TilesetSource& rhs) const {
		return numTiles == rhs.numTiles && tilesetFilename == rhs.tilesetFilename;
	}

	bool operator!=(const TilesetSource& rhs) const {
		return !(*this == rhs);
	}

	bool IsEmpty() const {
		return (numTiles == 0) || tilesetFilename.empty();
	}
};

#pragma pack(pop)
