#pragma once

#include <string>

#pragma pack(push, 1) // Make sure structures are byte aligned

// Facilitates finding the source BMP file (well00XX.bmp) for a tile set. 
// Tile set names must be exactly 8 chars long not including file extension.
struct TilesetSource
{
	// Fixed length string. Map and save files require tile set filenames to be exactly 8
	// characters long. User must handle terminating the string.
	std::string tilesetFilename;
	
	// Number of Tiles in set (represented on BMP).
	int tileCount;

	void Set(std::string filename, int tileCount);
	void Remove();

	bool IsSet()
	{
		return tileCount > 0;
	}
};

#pragma pack(pop)
