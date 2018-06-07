#include "MapReader.h"
#include "../StreamReader.h"
#include <iostream>
#include <stdexcept>
#include <cstring>

MapData MapReader::Read(std::string filename, bool savedGame)
{
	FileStreamReader mapReader(filename);
	return Read(mapReader, savedGame);
}

MapData MapReader::Read(SeekableStreamReader& mapStream, bool savedGame)
{
	streamReader = &mapStream;

	MapData mapData;

	if (savedGame) {
		SkipSaveGameHeader();
	}

	ReadHeader(mapData);
	ReadTiles(mapData);
	ReadClipRect(mapData.clipRect);
	ReadTilesetSources(mapData);
	ReadTilesetHeader();
	ReadTileInfo(mapData);
	ReadVersionTag();
	ReadVersionTag();
	ReadTileGroups(mapData);

	return mapData;
}

void MapReader::SkipSaveGameHeader()
{
	streamReader->SeekRelative(0x1E025);
}

void MapReader::ReadHeader(MapData& mapData)
{
	streamReader->Read(&mapData.header, sizeof(mapData.header));
}

void MapReader::ReadTiles(MapData& mapData)
{
	mapData.tiles.resize(mapData.header.TileCount());
	streamReader->Read(&mapData.tiles[0], mapData.tiles.size() * sizeof(TileData));
}

void MapReader::ReadClipRect(const ClipRect& clipRect)
{
	streamReader->Read(&clipRect, sizeof(clipRect));
}

void MapReader::ReadTilesetHeader()
{
	char buffer[10];
	streamReader->Read(buffer, sizeof(buffer));

	if (std::strncmp(buffer, "TILE SET\x1a", sizeof(buffer))) {
		throw std::runtime_error("'TILE SET' string not found.");
	}
}

void MapReader::ReadTilesetSources(MapData& mapData)
{
	mapData.tilesetSources.resize(static_cast<size_t>(mapData.header.numTilesets));

	for (unsigned int i = 0; i < mapData.header.numTilesets; ++i)
	{
		mapData.tilesetSources[i].tilesetFilename = ReadString();

		if (mapData.tilesetSources[i].tilesetFilename.size() > 8) {
			throw std::runtime_error("Tileset name may not be greater than 8 characters in length.");
		}

		if (mapData.tilesetSources[i].tilesetFilename.size() > 0) {
			streamReader->Read(&mapData.tilesetSources[i].numTiles, sizeof(int));
		}
	}
}

void MapReader::ReadTileInfo(MapData& mapData)
{
	size_t numTileInfo;
	streamReader->Read(&numTileInfo, sizeof(numTileInfo));

	mapData.tileInfos.resize(numTileInfo);
	streamReader->Read(&mapData.tileInfos[0], numTileInfo * sizeof(TileInfo));

	size_t numTerrainTypes;
	streamReader->Read(&numTerrainTypes, sizeof(numTerrainTypes));
	mapData.terrainTypes.resize(numTerrainTypes);
	streamReader->Read(&mapData.terrainTypes[0], numTerrainTypes * sizeof(TerrainType));
}

void MapReader::ReadVersionTag()
{
	int versionTag;
	streamReader->Read(&versionTag, sizeof(versionTag));

	if (versionTag < 0x1010)
	{
		std::cerr << "All instances of version tag in .map and .op2 files should be greater than 0x1010.";
	}
}

void MapReader::ReadTileGroups(MapData& mapData)
{
	int numTileGroups;
	streamReader->Read(&numTileGroups, sizeof(numTileGroups));
	streamReader->SeekRelative(4);

	for (int i = 0; i < numTileGroups; ++i)
	{
		mapData.tileGroups.push_back(ReadTileGroup());
	}
}

TileGroup MapReader::ReadTileGroup()
{
	TileGroup tileGroup;

	streamReader->Read(&tileGroup.tileWidth, sizeof(tileGroup.tileWidth));
	streamReader->Read(&tileGroup.tileHeight, sizeof(tileGroup.tileHeight));

	int mappingIndex;
	for (int i = 0; i < tileGroup.tileHeight * tileGroup.tileWidth; ++i)
	{
		streamReader->Read(&mappingIndex, sizeof(mappingIndex));
		tileGroup.mappingIndices.push_back(mappingIndex);
	}

	tileGroup.name = ReadString();

	return tileGroup;
}

// String must be stored in file as string length followed by char[].
std::string MapReader::ReadString()
{
	size_t stringLength;
	streamReader->Read(&stringLength, sizeof(stringLength));

	if (stringLength == 0) {
		return std::string();
	}

	char* cString = new char[stringLength];

	try
	{
		streamReader->Read(cString, stringLength);
		std::string s(cString, stringLength);
		delete cString;

		return s;
	}
	catch (std::exception e)
	{
		delete cString;

		throw e;
	}
}
