#pragma once

#include "StreamReader.h"
#include <vector>
#include <cmath>
#include <string>

// Includes data structures to represent a map or saved game in Outpost 2.

// FILE FORMAT DOCUMENTATION: 
//     Outpost2SVN\OllyDbg\InternalData\FileFormat SavedGame and Map.txt.

// ALT IMPLEMENTATION (with COM support)
//     Outpost2SVN\MapEditor\OP2Editor.
#pragma pack(push, 1)				// Make sure structures are byte aligned
struct MapHeader
{
	// The map's version tag. 
	// It must be >= to 0x1010 or Outpost 2 will abort loading the map.
	int versionTag;

	// True if file represents a saved game instead of a map file.
	int bSavedGame;

	// Width of map in tiles in base 2 Logarithm representation. 
	// Outpost 2 maps must be representable by a base 2 Logarithm.
	// See function MapTileWidth.
	unsigned int lgMapTileWidth; 
	
	// Height of map in tiles. Will be rounded to a power of 2 by Outpost 2.
	unsigned int mapTileHeight;

	// Number of tile sets within the map.
	unsigned int numTileSets;

	// Map Width in Tiles.
	unsigned int mapTileWidth()
	{
		return (unsigned int)pow(2, lgMapTileWidth);
	}
};

// Bitfield structure (32 bits total) containing metadata for a tile placed on the map.
struct TileData
{
	// Determines movement speed of tile, or if tile is bulldozed, walled, tubed, or has rubble.
	int cellType : 5;

	// The tile's associated index in the TileInfo array. 
	// TileInfo lists general properties associated with a tile.
	unsigned int tileIndex : 11;

	// Index of the unit that occupies the tile.
	int unitIndex : 11;

	// True if lava is present on tile.
	int bLava : 1;

	// True if it is possible for lava to enter the tile.
	int bLavaPossible : 1;

	// Used in controlling Lava and Microbe spread. exact workings UNKNOWN.
	int bExpansion : 1;

	// True if the Blight is present on tile.
	int bMicrobe : 1;

	// True if a wall or a building has been built on the tile.
	int bWallOrBuilding : 1;
};

// Represents playable area of the map.
struct ClipRect
{
	int x1;
	int y1;
	int x2;
	int y2;
};

// Contains information to find the source BMP file (well00XX.bmp) for a tile set.
struct TileSetSource
{
	// Fixed length string. Map and save files require tile set filenames to be exactly 8
	// characters long. User must handle terminating the string.
	char tileSetFilename[8]; 

	// Number of Tiles in set (represented on BMP).
	int numTiles;

	std::string getTileSetFilename()
	{
		std::string s;
		for (int i = 0; i < 8; ++i)
			s.push_back(tileSetFilename[i]);
	    
		return s;
	}
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
	MapData(SeekableStreamReader* mapStream, bool saveGame = false);
	MapData(std::string filename, bool saveGame = false) : MapData(&FileStreamReader(filename), saveGame) {}

	unsigned int getTileInfoIndex(unsigned int x, unsigned int y);
	int getCellType(unsigned int x, unsigned int y);
	int getLavaPossible(unsigned int x, unsigned int y);
	short getTileSetIndex(unsigned int x, unsigned int y);
	short getImageIndex(unsigned int x, unsigned int y);

private:
	size_t GetCellIndex(unsigned int x, unsigned int y);

	void skipSaveGameHeader(SeekableStreamReader* streamReader);
	void readMapHeader(StreamReader* streamReader);
	void readTileData(StreamReader* streamReader);
	void readClipRect(StreamReader* streamReader);
	void readTileSetSources(StreamReader* streamReader);
	void readTileSetHeader(StreamReader* streamReader);
	void readTileInfo(StreamReader* streamReader);
};

#pragma pack(pop)