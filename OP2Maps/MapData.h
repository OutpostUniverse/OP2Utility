#pragma once

#include "MapHeader.h"
#include "TileData.h"
#include "TileSetSource.h"
#include "../StreamReader.h"
#include <string>
#include <vector>
#include <memory>

// Includes data structures to represent a map or saved game in Outpost 2.

// FILE FORMAT DOCUMENTATION: 
//     Outpost2SVN\OllyDbg\InternalData\FileFormat SavedGame and Map.txt.

// ALT IMPLEMENTATION (with COM support)
//     Outpost2SVN\MapEditor\OP2Editor.

#pragma pack(push, 1) // Make sure structures are byte aligned

// Represents playable area of the map.
struct ClipRect
{
	int x1;
	int y1;
	int x2;
	int y2;
};

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

#pragma pack(pop)
