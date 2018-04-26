#pragma once

#include "MapHeader.h"
#include "TileData.h"
#include "TileSetSource.h"
#include "TileInfo.h"
#include "TerrainType.h"
#include "ClipRect.h"
#include "TileGroup.h"
#include <string>
#include <vector>


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
	unsigned int GetTileInfoIndex(unsigned int x, unsigned int y) const;
	int GetCellType(unsigned int x, unsigned int y) const;
	int GetLavaPossible(unsigned int x, unsigned int y) const;
	short GetTilesetIndex(unsigned int x, unsigned int y) const;
	short GetImageIndex(unsigned int x, unsigned int y) const;

	// Fills tileset's associated TileInfos with default values.
	void AddTileset(std::string filename, int tileCount);

	// If tileset is not found, function exits without throwing an error.
	// Only removes the tileset if it does not belog to a TerrainType.
	// After removingtileset:
	//   1. Sets all tiles on the map that were associated with the tileset to index 0 (typically blue)
	//   2. Sets all tiles on TileGroups that were associated with the tileset to index 0
	void RemoveTileset(std::string filename);
	
private:
	size_t GetCellIndex(unsigned int x, unsigned int y) const;

	void AddTilesetSource(std::string filename, int tileCount);
	size_t GetTilesetIndex(std::string filename);
	void RemoveTilesetSource(size_t tilesetIndex);
	void RemoveTileInfos(size_t tilesetIndex);
	void UpdateTilesAfterTilesetRemoval(const TileRange& tileRange);
	void UpdateTileGroupsAfterTilesetRemoval(const TileRange& tileRange);
	void UpdateTerrainTypesAfterTilesetRemoval(const TileRange& rangeRemoved, std::string tilesetName);
};
