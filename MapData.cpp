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
		skipSaveGameHeader(*mapStream);

	readMapHeader(*mapStream);
	readTileData(*mapStream);
	readClipRect(*mapStream);
	readTileSetSources(*mapStream);
	readTileSetHeader(*mapStream);
	readTileInfo(*mapStream);
}

void MapData::skipSaveGameHeader(SeekableStreamReader& streamReader)
{
	streamReader.ignore(0x1E025);
}

void MapData::readMapHeader(StreamReader& streamReader)
{
	streamReader.read((char*)&mapHeader, sizeof(mapHeader));
}

void MapData::readTileData(StreamReader& streamReader)
{
	tileDataVector.resize(mapHeader.mapTileHeight << mapHeader.lgMapTileWidth);
	streamReader.read((char*)&tileDataVector[0], tileDataVector.size() * sizeof(TileData));
}

void MapData::readClipRect(StreamReader& streamReader)
{
	streamReader.read((char*)&clipRect, sizeof(clipRect));
}

void MapData::readTileSetHeader(StreamReader& streamReader)
{
	char buffer[10];
	streamReader.read(buffer, sizeof(buffer));

	if (strncmp(buffer, "TILE SET\x1a", sizeof(buffer)))
		throw runtime_error("'TILE SET' string not found.");
}

void MapData::readTileSetSources(StreamReader& streamReader)
{
	tileSetSources.resize(static_cast<size_t>(mapHeader.numTileSets));

	for (unsigned int i = 0; i < mapHeader.numTileSets; ++i)
	{
		size_t stringLength;
		streamReader.read((char*)&stringLength, sizeof(stringLength));

		if (stringLength > 8)
			throw runtime_error("Tile Set Name greater than 8 characters in length.");

		if (stringLength == 0)
			continue;

		streamReader.read((char*)&tileSetSources[i].tileSetFilename, stringLength);

		streamReader.read((char*)&tileSetSources[i].numTiles, sizeof(int));
	}
}

void MapData::readTileInfo(StreamReader& streamReader)
{
	size_t numTileInfo;
	streamReader.read((char*)&numTileInfo, sizeof(numTileInfo));

	tileInfoVector.resize(numTileInfo);
	streamReader.read((char*)&tileInfoVector[0], numTileInfo * sizeof(TileInfo));

	size_t numTerrainTypes;
	streamReader.read((char*)&numTerrainTypes, sizeof(numTerrainTypes));
	terrainTypeVector.resize(numTerrainTypes);
	streamReader.read((char*)&terrainTypeVector[0], numTerrainTypes * sizeof(TerrainType));
}

unsigned int MapData::getTileInfoIndex(unsigned int x, unsigned int y)
{
	size_t cellIndex = GetCellIndex(x, y);
	return tileDataVector[cellIndex].tileIndex;
}

int MapData::getCellType(unsigned int x, unsigned int y)
{
	return tileDataVector[GetCellIndex(x, y)].cellType;
}

int MapData::getLavaPossible(unsigned int x, unsigned int y)
{
	return tileDataVector[GetCellIndex(x, y)].bLavaPossible;
}

short MapData::getTileSetIndex(unsigned int x, unsigned int y)
{
	unsigned int tileInfoIndex = getTileInfoIndex(x, y);
	return tileInfoVector[tileInfoIndex].tileSetIndex;
}

short MapData::getImageIndex(unsigned int x, unsigned int y)
{
	unsigned int tileInfoIndex = getTileInfoIndex(x, y);
	return tileInfoVector[tileInfoIndex].tileIndex;
}

size_t MapData::GetCellIndex(unsigned int x, unsigned int y)
{
	unsigned int lowerX = x & 0x1F; // ... 0001 1111
	unsigned int upperX = x >> 5;   // ... 1110 0000
	return (upperX * mapHeader.mapTileHeight + y) * 32 + lowerX;
}
