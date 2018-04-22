#pragma once

#pragma pack(push, 1) // Make sure structure is byte aligned

// Outpost 2 map and save file header
struct MapHeader
{
	// The map's version tag. 
	// It must be >= to 0x1010 or Outpost 2 will abort loading the map.
	int versionTag;

	// True if file represents a saved game instead of a map file.
	int bSavedGame;

	// Width of map in tiles in base 2 Logarithm representation. 
	// Outpost 2 maps must be representable by a base 2 Logarithm.
	// See function MapTileWidth.
	unsigned int lgMapTileWidth;

	// Height of map in tiles. Will be rounded to a power of 2 by Outpost 2.
	unsigned int mapTileHeight;

	// Number of tile sets within the map.
	unsigned int numTileSets;

	// Map Width in Tiles.
	unsigned int MapTileWidth()
	{
		return  1 << lgMapTileWidth; // Use bitwise left shift to translate base 2 logarithm.
	}
};

#pragma pack(pop)
