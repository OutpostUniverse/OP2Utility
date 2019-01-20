#pragma once

#include "CellType.h"

#pragma pack(push, 1) // Make sure structures are byte aligned

// Outpost 2 Tile metadata. Implemented as a Bitfield structure (32 bits total)
struct Tile
{
	// Determines movement speed of tile, or if tile is bulldozed, walled, tubed, or has rubble.
	CellType cellType : 5;

	// TileMapping lists graphics and animation properties associated with a tile.
	unsigned int tileMappingIndex : 11;

	// Index of the unit that occupies the tile.
	int unitIndex : 11;

	// True if lava is present on tile.
	int bLava : 1;

	// True if it is possible for lava to enter the tile.
	int bLavaPossible : 1;

	// Used in controlling Lava and Microbe spread. exact workings UNKNOWN.
	int bExpansion : 1;

	// True if the Blight is present on tile.
	int bMicrobe : 1;

	// True if a wall or a building has been built on the tile.
	int bWallOrBuilding : 1;
};

static_assert(4 == sizeof(Tile), "Tile is an unexpected size");

#pragma pack(pop)
