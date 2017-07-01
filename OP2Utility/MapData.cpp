#include "MapData.h"
#include <iostream>
#include <fstream>

using namespace std;

MapData::MapData(string filename, bool saveGame)
{
	ifstream file(filename, ios::in | ios::binary);

	if (!file.is_open())
		throw exception("Map file could not be opened.");
	
	if (saveGame)
		skipSaveGameHeader(file);

	readMapHeader(file);
	readTileData(file);
	readClipRect(file);
	readTileSetSources(file);
	readTileSetHeader(file);
	readTileInfo(file);

	file.close();
}

void MapData::skipSaveGameHeader(ifstream& file)
{
	file.ignore(0x1E025);
}

void MapData::readMapHeader(ifstream& file)
{
	file.read((char*)&mapHeader, sizeof(mapHeader));
}

void MapData::readTileData(ifstream& file)
{
	tileDataVector.resize(mapHeader.mapTileHeight << mapHeader.lgMapTileWidth);
	file.read((char*)&tileDataVector[0], tileDataVector.size() * sizeof(TileData));
}

void MapData::readClipRect(ifstream& file)
{
	file.read((char*)&clipRect, sizeof(clipRect));
}

void MapData::readTileSetHeader(ifstream& file)
{
	char buffer[10];
	file.read(buffer, sizeof(buffer));
	if (strncmp(buffer, "TILE SET\x1a", sizeof(buffer)))
	{
		throw exception("'TILE SET' string not found.");
	}
}

void MapData::readTileSetSources(ifstream& file)
{
	tileSetSources.resize((size_t)mapHeader.numTileSets);

	for (unsigned int i = 0; i < mapHeader.numTileSets; ++i)
	{
		size_t stringLength;
		file.read((char*)&stringLength, sizeof(stringLength));

		if (stringLength > 8)
			throw exception("Tile Set Name greater than 8 characters in length.");

		if (stringLength == 0)
			continue;

		file.read((char*)&tileSetSources[i].tileSetFilename, stringLength);

		file.read((char*)&tileSetSources[i].numTiles, sizeof(int));
	}
}

void MapData::readTileInfo(ifstream& file)
{
	size_t numTileInfo;
	file.read((char*)&numTileInfo, sizeof(numTileInfo));

	tileInfoVector.resize(numTileInfo);
	file.read((char*)&tileInfoVector[0], numTileInfo * sizeof(TileInfo));

	size_t numTerrainTypes;
	file.read((char*)&numTerrainTypes, sizeof(numTerrainTypes));
	terrainTypeVector.resize(numTerrainTypes);
	file.read((char*)&terrainTypeVector[0], numTerrainTypes * sizeof(TerrainType));
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