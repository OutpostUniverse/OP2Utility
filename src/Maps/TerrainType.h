#pragma once

#include <cstdint>

#pragma pack(push, 1) // Make sure structures are byte aligned

struct TileRange
{
	// First tile index in range.
	uint16_t start;

	// Last tile index in range.
	uint16_t end;
};

static_assert(4 == sizeof(TileRange), "TileRange is an unexpected size");

// The properties associated with a range of tiles.
struct TerrainType
{
	// Range of tiles the TerrainType applies to.
	TileRange tileRange;

	// The first tile index that represents bulldozed versions of the terrain type.
	// The rest of the bulldozed tiles appear consecutively after the first index.
	uint16_t bulldozedTileIndex;

	// The first tile index of rubble for the Terrain Type range.
	// Common rubble will be the first 4 consecutive tiles after index.
	// Rare rubble will be the next 4 consecutive tiles.
	uint16_t rubbleTileIndex;

	// Index of tube tiles. UNKOWN why the data is repeated.
	uint16_t tubeTiles[6];

	// 5 groups of 16 tiles. Each group represents a different wall type.
	// Lava, Microbe, Full Strength Regular, Damaged Regular, and Heavily Damaged Regular.
	int16_t wall[5][16];

	// First index for lava tiles in Terrain Type.
	uint16_t lavaTileIndex;

	// UNKOWN
	uint16_t flat1;

	// UNKNOWN
	uint16_t flat2;

	// UNKNOWN
	uint16_t flat3;

	// Tube tile indices associated with Terrain Type.
	uint16_t tube[16];

	// Index for scorched tile within Terrain Type
	// Scorch comes from meteor impact or vehicle destruction. Not all tiles may be scorched.
	uint16_t scorchedTileIndex;

	TileRange scorchedRange[3];

	// UNKNOWN
	int16_t unkown[15];
};

static_assert(248 + 4 * sizeof(TileRange) == sizeof(TerrainType), "TerrainType is an unexpected size");

#pragma pack(pop)
