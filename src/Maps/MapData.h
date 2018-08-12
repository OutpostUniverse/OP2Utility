#pragma once

#include "MapHeader.h"
#include "TileData.h"
#include "TilesetSource.h"
#include "TileInfo.h"
#include "TerrainType.h"
#include "ClipRect.h"
#include "TileGroup.h"
#include <string>
#include <vector>
#include <cstddef>
#include <cstdint>

enum class CellType;

// FILE FORMAT DOCUMENTATION:
//     Outpost2SVN\OllyDbg\InternalData\FileFormat SavedGame and Map.txt.

// ALT IMPLEMENTATION (with COM support)
//     Outpost2SVN\MapEditor\OP2Editor.

//Represents the data stored in an Outpost 2 map file.
struct MapData
{
	MapHeader header;

	// 1D listing of all tiles on the associated map. See MapHeader data for height and width of map.
	std::vector<TileData> tiles;

	ClipRect clipRect;

	// Listing of all tile set sources associated with the map.
	std::vector<TilesetSource> tilesetSources;

	// Metadata about each available tile from the tile set sources.
	std::vector<TileInfo> tileInfos;

	// Listing of properties grouped by terrain type. Properties apply to a given range of tiles.
	std::vector<TerrainType> terrainTypes;

	std::vector<TileGroup> tileGroups;

public:
	unsigned int GetTileInfoIndex(std::size_t x, std::size_t y) const;
	CellType GetCellType(std::size_t x, std::size_t y) const;
	int GetLavaPossible(std::size_t x, std::size_t y) const;
	uint16_t GetTilesetIndex(std::size_t x, std::size_t y) const;
	uint16_t GetImageIndex(std::size_t x, std::size_t y) const;

private:
	std::size_t GetTileIndex(std::size_t x, std::size_t y) const;
};
