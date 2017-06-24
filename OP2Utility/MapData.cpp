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
		SkipSaveGameHeader(file);

	ReadMapHeader(file);
	ReadTileData(file);
	ReadClipRect(file);
	ReadTileSetSources(file);
	ReadTileSetHeader(file);
	ReadTileInfo(file);

	file.close();
}

void MapData::SkipSaveGameHeader(ifstream& file)
{
	file.ignore(0x1E025);
}

void MapData::ReadMapHeader(ifstream& file)
{
	file.read((char*)&mapHeader, sizeof(mapHeader));
}

void MapData::ReadTileData(ifstream& file)
{
	tileDataVector.resize(mapHeader.mapTileHeight << mapHeader.lgMapTileWidth);
	file.read((char*)&tileDataVector[0], tileDataVector.size() * sizeof(TileData));
}

void MapData::ReadClipRect(ifstream& file)
{
	file.read((char*)&clipRect, sizeof(clipRect));
}

void MapData::ReadTileSetHeader(ifstream& file)
{
	char buffer[10];
	file.read(buffer, sizeof(buffer));
	if (strncmp(buffer, "TILE SET\x1a", sizeof(buffer)))
	{
		throw exception("'TILE SET' string not found.");
	}
}

void MapData::ReadTileSetSources(ifstream& file)
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

void MapData::ReadTileInfo(ifstream& file)
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