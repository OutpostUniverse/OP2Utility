#pragma once

#pragma pack(push, 1) // Make sure structures are byte aligned

// Metadata that applies to all tiles on a map with the same TileSet and TileIndex.
struct TileInfo
{
	// The tile set the tile comes from.
	short tileSetIndex;

	// The tile index within the tile set. 
	short tileIndex;

	// The number of tiles following this index that may be used to represent the tile 
	// for an animation sequence. 
	short numAnimations;

	// The number of cycles elapsed before cycling to the next tile in an animation.
	short animationDelay;
};

#pragma pack(pop)