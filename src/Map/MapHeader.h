#pragma once

#include <cstdint>

#pragma pack(push, 1) // Make sure structure is byte aligned

// Outpost 2 map and save file header
struct MapHeader
{
	MapHeader();

	// Minimum map version tag that Outpost 2 will accept without error
	static const uint32_t MinMapVersion = 0x1010;

	// Default map version tag used by original Outpost 2 maps
	static const uint32_t CurrentMapVersion = 0x1011;

	// The map's version tag must be >= MinMapVersion or Outpost 2 will abort loading the map.
	uint32_t versionTag;

	// True if file represents a saved game instead of a map file.
	int32_t bSavedGame;

	// Width of map in tiles in base 2 Logarithm representation.
	// Outpost 2 maps must be representable by a base 2 Logarithm.
	// See function WidthInTiles.
	uint32_t lgWidthInTiles;

	// Height of map in tiles. Will be rounded to a power of 2 by Outpost 2.
	uint32_t heightInTiles;

	// Number of tile sets on map.
	uint32_t tilesetCount;

	// Map Width in Tiles.
	uint32_t WidthInTiles() const
	{
		return  1 << lgWidthInTiles; // Use bitwise left shift to translate base 2 logarithm.
	}

	bool VersionTagValid() const
	{
		return versionTag >= MinMapVersion;
	}

	// Total number of tiles on map.
	uint32_t TileCount() const
	{
		return heightInTiles << lgWidthInTiles;
	}
};

static_assert(20 == sizeof(MapHeader), "MapHeader is an unexpected size");

#pragma pack(pop)
