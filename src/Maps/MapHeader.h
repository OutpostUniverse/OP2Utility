#pragma once

#include <cstdint>

#pragma pack(push, 1) // Make sure structure is byte aligned

// Outpost 2 map and save file header
struct MapHeader
{
	MapHeader();

	// Minimum map version tag that Outpost 2 will accept without error
	static const uint32_t MinMapVersion = 0x1010;

	// The map's version tag.
	// It must be >= to 0x1010 or Outpost 2 will abort loading the map.
	int32_t versionTag;

	// True if file represents a saved game instead of a map file.
	int32_t bSavedGame;

	// Width of map in tiles in base 2 Logarithm representation.
	// Outpost 2 maps must be representable by a base 2 Logarithm.
	// See function MapTileWidth.
	uint32_t lgMapTileWidth;

	// Height of map in tiles. Will be rounded to a power of 2 by Outpost 2.
	uint32_t mapTileHeight;

	// Number of tile sets on map.
	uint32_t tilesetCount;

	// Map Width in Tiles.
	uint32_t MapTileWidth() const
	{
		return  1 << lgMapTileWidth; // Use bitwise left shift to translate base 2 logarithm.
	}

	bool VersionTagValid() const
	{
		return versionTag >= MinMapVersion;
	}

	// Total number of tiles on map.
	uint32_t TileCount() const
	{
		return mapTileHeight << lgMapTileWidth;
	}
};

static_assert(20 == sizeof(MapHeader), "MapHeader is an unexpected size");

#pragma pack(pop)
