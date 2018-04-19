#pragma once

#include "MapHeader.h"
#include "TileData.h"
#include "TileSetSource.h"
#include "TileInfo.h"
#include "TerrainType.h"
#include "ClipRect.h"
#include "../StreamReader.h"
#include <string>
#include <vector>
#include <memory>

// Includes data structures to represent a map or saved game in Outpost 2.

// FILE FORMAT DOCUMENTATION: 
//     Outpost2SVN\OllyDbg\InternalData\FileFormat SavedGame and Map.txt.

// ALT IMPLEMENTATION (with COM support)
//     Outpost2SVN\MapEditor\OP2Editor.

//Represents the data stored in an Outpost 2 map file. 
struct MapData
{
	MapHeader mapHeader;

	// 1D listing of all tiles on the associated map. See MapHeader data for height and width of map.
	std::vector<TileData> tileDataVector;

	ClipRect clipRect;

	// Listing of all tile set sources associated with the map.
	std::vector<TileSetSource> tileSetSources;

	// Metadata about each available tile from the tile set sources.
	std::vector<TileInfo> tileInfoVector;

	// Listing of properties grouped by terrain type. Properties apply to a given range of tiles.
	std::vector<TerrainType> terrainTypeVector;

public:
	MapData(std::unique_ptr<SeekableStreamReader> mapStream, bool saveGame = false);
	MapData(std::string filename, bool saveGame = false) : MapData(std::make_unique<FileStreamReader>(filename), saveGame) {}

	unsigned int GetTileInfoIndex(unsigned int x, unsigned int y);
	int GetCellType(unsigned int x, unsigned int y);
	int GetLavaPossible(unsigned int x, unsigned int y);
	short GetTileSetIndex(unsigned int x, unsigned int y);
	short GetImageIndex(unsigned int x, unsigned int y);

private:
	size_t GetCellIndex(unsigned int x, unsigned int y);

	void SkipSaveGameHeader(SeekableStreamReader& streamReader);
	void ReadMapHeader(StreamReader& streamReader);
	void ReadTileData(StreamReader& streamReader);
	void ReadClipRect(StreamReader& streamReader);
	void ReadTileSetSources(StreamReader& streamReader);
	void ReadTileSetHeader(StreamReader& streamReader);
	void ReadTileInfo(StreamReader& streamReader);
};
