#pragma once

#include <cstdint>

#pragma pack(push, 1) // Make sure structures are byte aligned

struct Range16
{
	uint16_t start;
	uint16_t end;
};

static_assert(4 == sizeof(Range16), "Range16 is an unexpected size");

// The properties associated with a range of tiles.
struct TerrainType
{
	// Range of tile mappings the TerrainType applies to.
	Range16 tileMappingRange;

	// The first tile mapping index that represents bulldozed versions of the terrain type.
	// The rest of the bulldozed tiles appear consecutively after the first index.
	uint16_t bulldozedTileMappingIndex;

	// The first tile mapping index of rubble for the Terrain Type range.
	// Common rubble will be the first 4 consecutive tile mappings after index.
	// Rare rubble will be the next 4 consecutive tile mappings.
	uint16_t rubbleTileMappingIndex;

	// Mapping index of tube tiles. UNKNOWN why the data is repeated.
	uint16_t tubeTileMappings[6];

	// 5 groups of 16 tiles. Each group represents a different wall type.
	// Lava, Microbe, Full Strength Regular, Damaged Regular, and Heavily Damaged Regular.
	uint16_t wallTileMappingIndexes[5][16];

	// First mapping index for lava tiles in Terrain Type.
	uint16_t lavaTileMappingIndex;

	// UNKNOWN
	uint16_t flat1;

	// UNKNOWN
	uint16_t flat2;

	// UNKNOWN
	uint16_t flat3;

	// Tube tile mapping indexes associated with Terrain Type.
	uint16_t tubeTileMappingIndexes[16];

	// Index for scorched tile within Terrain Type
	// Scorch comes from meteor impact or vehicle destruction. Not all tiles may be scorched.
	uint16_t scorchedTileMappingIndex;

	Range16 scorchedRange[3];

	// UNKNOWN
	int16_t unknown[15];
};

static_assert(264 == sizeof(TerrainType), "TerrainType is an unexpected size");

#pragma pack(pop)
