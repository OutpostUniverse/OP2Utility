#pragma once

#include "CellType.h"

#pragma pack(push, 1) // Make sure structures are byte aligned

// Outpost 2 Tile metadata. Implemented as a Bitfield structure (32 bits total)
struct TileData
{
	// Determines movement speed of tile, or if tile is bulldozed, walled, tubed, or has rubble.
	//int cellType : 5;
	CellType cellType : 5;

	// The tile's associated index in the TileInfo array.
	// TileInfo lists general properties associated with a tile.
	unsigned int tileIndex : 11;

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

#pragma pack(pop)
