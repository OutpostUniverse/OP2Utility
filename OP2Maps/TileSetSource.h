#pragma once

#include <string>

#pragma pack(push, 1) // Make sure structures are byte aligned

// Facilitates finding the source BMP file (well00XX.bmp) for a tile set. 
// Tile set names must be exactly 8 chars long not including file extension.
struct TileSetSource
{
	// Fixed length string. Map and save files require tile set filenames to be exactly 8
	// characters long. User must handle terminating the string.
	char tileSetFilename[8]; 

	// Number of Tiles in set (represented on BMP).
	int numTiles;

	std::string GetTileSetFilename()
	{
		// Constructing string with a char[] without specifying range adds an extra char to the end of the string.
		return std::string(tileSetFilename, 8);
	}
};

#pragma pack(pop)
