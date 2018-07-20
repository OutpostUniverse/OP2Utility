#pragma once

#include <cstdint>
#include <vector>
#include <string>

// A set of tiles that group together to form a feature in Outpost 2, such as a large rock or cliff.
struct TileGroup
{
	std::string name;

	// Width of tile group in tiles
	uint32_t tileWidth;

	// Height of tile group in tiles
	uint32_t tileHeight;

	// Tiles used in TileGroup listed in 1D (all of row 0 tiles first, then row 1, etc).
	std::vector<uint32_t> mappingIndices;
};
