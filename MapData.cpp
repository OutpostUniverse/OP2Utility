#include "MapData.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <stdexcept>

using namespace std;

MapData::MapData(unique_ptr<SeekableStreamReader> mapStream, bool saveGame)
{
	if (!mapStream)
		throw runtime_error("Provided map or save stream does not exist or is malformed.");

	if (saveGame)
		SkipSaveGameHeader(*mapStream);

	ReadMapHeader(*mapStream);
	ReadTileData(*mapStream);
	ReadClipRect(*mapStream);
	ReadTileSetSources(*mapStream);
	ReadTileSetHeader(*mapStream);
	ReadTileInfo(*mapStream);
}

void MapData::SkipSaveGameHeader(SeekableStreamReader& streamReader)
{
	streamReader.Ignore(0x1E025);
}

void MapData::ReadMapHeader(StreamReader& streamReader)
{
	streamReader.Read((char*)&mapHeader, sizeof(mapHeader));
}

void MapData::ReadTileData(StreamReader& streamReader)
{
	tileDataVector.resize(mapHeader.mapTileHeight << mapHeader.lgMapTileWidth);
	streamReader.Read((char*)&tileDataVector[0], tileDataVector.size() * sizeof(TileData));
}

void MapData::ReadClipRect(StreamReader& streamReader)
{
	streamReader.Read((char*)&clipRect, sizeof(clipRect));
}

void MapData::ReadTileSetHeader(StreamReader& streamReader)
{
	char buffer[10];
	streamReader.Read(buffer, sizeof(buffer));

	if (strncmp(buffer, "TILE SET\x1a", sizeof(buffer)))
		throw runtime_error("'TILE SET' string not found.");
}

void MapData::ReadTileSetSources(StreamReader& streamReader)
{
	tileSetSources.resize(static_cast<size_t>(mapHeader.numTileSets));

	for (unsigned int i = 0; i < mapHeader.numTileSets; ++i)
	{
		size_t stringLength;
		streamReader.Read((char*)&stringLength, sizeof(stringLength));

		if (stringLength > 8)
			throw runtime_error("Tile Set Name greater than 8 characters in length.");

		if (stringLength == 0)
			continue;

		streamReader.Read((char*)&tileSetSources[i].tileSetFilename, stringLength);

		streamReader.Read((char*)&tileSetSources[i].numTiles, sizeof(int));
	}
}

void MapData::ReadTileInfo(StreamReader& streamReader)
{
	size_t numTileInfo;
	streamReader.Read((char*)&numTileInfo, sizeof(numTileInfo));

	tileInfoVector.resize(numTileInfo);
	streamReader.Read((char*)&tileInfoVector[0], numTileInfo * sizeof(TileInfo));

	size_t numTerrainTypes;
	streamReader.Read((char*)&numTerrainTypes, sizeof(numTerrainTypes));
	terrainTypeVector.resize(numTerrainTypes);
	streamReader.Read((char*)&terrainTypeVector[0], numTerrainTypes * sizeof(TerrainType));
}

unsigned int MapData::GetTileInfoIndex(unsigned int x, unsigned int y)
{
	size_t cellIndex = GetCellIndex(x, y);
	return tileDataVector[cellIndex].tileIndex;
}

int MapData::GetCellType(unsigned int x, unsigned int y)
{
	return tileDataVector[GetCellIndex(x, y)].cellType;
}

int MapData::GetLavaPossible(unsigned int x, unsigned int y)
{
	return tileDataVector[GetCellIndex(x, y)].bLavaPossible;
}

short MapData::GetTileSetIndex(unsigned int x, unsigned int y)
{
	unsigned int tileInfoIndex = GetTileInfoIndex(x, y);
	return tileInfoVector[tileInfoIndex].tileSetIndex;
}

short MapData::GetImageIndex(unsigned int x, unsigned int y)
{
	unsigned int tileInfoIndex = GetTileInfoIndex(x, y);
	return tileInfoVector[tileInfoIndex].tileIndex;
}

size_t MapData::GetCellIndex(unsigned int x, unsigned int y)
{
	unsigned int lowerX = x & 0x1F; // ... 0001 1111
	unsigned int upperX = x >> 5;   // ... 1110 0000
	return (upperX * mapHeader.mapTileHeight + y) * 32 + lowerX;
}
