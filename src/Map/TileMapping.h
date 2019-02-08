#pragma once

#include <cstdint>

#pragma pack(push, 1) // Make sure structures are byte aligned

// Metadata that applies to all tiles on a map with the same Tileset and TileIndex.
struct TileMapping
{
	// The tile set the tile comes from.
	uint16_t tilesetIndex;

	// The tile index within the tile set.
	uint16_t tileGraphicIndex;

	// The number of tiles following this index that may be used to represent the tile
	// for an animation sequence.
	uint16_t animationCount;

	// The number of cycles elapsed before cycling to the next tile in an animation.
	uint16_t animationDelay;
};

static_assert(8 == sizeof(TileMapping), "TileMapping is an unexpected size");

#pragma pack(pop)
