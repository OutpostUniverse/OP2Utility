#pragma once

#pragma pack(push, 1) // Make sure structures are byte aligned

struct TileRange
{
	// First tile index in range.
	short start;

	// Last tile index in range.
	short end;
};

// The properties associated with a range of tiles.
struct TerrainType
{
	// Range of tiles the TerrainType applies to.
	TileRange tileRange;

	// The first tile index that represents bulldozed versions of the terrain type.
	// The rest of the bulldozed tiles appear consecutively after the first index.
	short bulldozedTileIndex;

	// The first tile index of rubble for the Terrain Type range. 
	// Common rubble will be the first 4 consecutive tiles after index.
	// Rare rubble will be the next 4 consecutive tiles.
	short rubbleTileIndex;

	// Index of tube tiles. UNKOWN why the data is repeated.
	short tubeTiles[6];

	// 5 groups of 16 tiles. Each group represents a different wall type.
	// Lava, Microbe, Full Strength Regular, Damaged Regular, and Heavily Damaged Regular.
	short wall[5][16];

	// First index for lava tiles in Terrain Type.
	short lavaTileIndex;

	// UNKOWN
	short flat1;

	// UNKNOWN
	short flat2;

	// UNKNOWN
	short flat3;

	// Tube tile indices associated with Terrain Type.
	short tube[16];

	// Index for scorched tile within Terrain Type 
	// Scorch comes from meteor impact or vehicle destruction. Not all tiles may be scorched.
	short scorchedTileIndex;

	TileRange scorchedRange[3];

	// UNKNOWN
	short unkown[15];
};

#pragma pack(pop)
