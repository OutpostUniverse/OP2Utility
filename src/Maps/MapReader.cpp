#include "MapData.h"
#include "../Streams/FileReader.h"
#include <iostream>
#include <stdexcept>
#include <array>
#include <cstring>

const std::array<char, 10> tilesetHeader{ "TILE SET\x1a" };

MapData MapData::ReadMap(std::string filename)
{
	Stream::FileReader mapReader(filename);
	return ReadMap(mapReader);
}

MapData MapData::ReadMap(Stream::SeekableReader& streamReader)
{
	MapHeader mapHeader;
	streamReader.Read(mapHeader);
		
	MapData mapData(mapHeader);
		
	mapData.tiles.resize(mapHeader.TileCount());
	streamReader.Read(mapData.tiles);

	streamReader.Read(mapData.clipRect);
	ReadTilesetSources(streamReader, mapData, static_cast<std::size_t>(mapHeader.tilesetCount));
	ReadTilesetHeader(streamReader);
	streamReader.Read<uint32_t>(mapData.tileInfos);
	streamReader.Read<uint32_t>(mapData.terrainTypes);
	ReadVersionTag(streamReader);
	ReadVersionTag(streamReader);
	ReadTileGroups(streamReader, mapData);

	return mapData;
}

MapData MapData::ReadSavedGame(std::string filename)
{
	Stream::FileReader mapReader(filename);
	return ReadSavedGame(mapReader);
}

MapData MapData::ReadSavedGame(Stream::SeekableReader& streamReader)
{
	SkipSaveGameHeader(streamReader);
	return ReadMap(streamReader);
}


// == Private methods ==

void MapData::SkipSaveGameHeader(Stream::SeekableReader& streamReader)
{
	streamReader.SeekRelative(0x1E025);
}

void MapData::ReadTilesetHeader(Stream::Reader& streamReader)
{
	std::array<char, 10> buffer;
	streamReader.Read(buffer);

	if (buffer != tilesetHeader) {
		throw std::runtime_error("'TILE SET' string not found.");
	}
}

void MapData::ReadTilesetSources(Stream::Reader& streamReader, MapData& mapData, std::size_t tilesetCount)
{
	mapData.tilesetSources.resize(tilesetCount);

	for (auto& tilesetSource : mapData.tilesetSources)
	{
		streamReader.Read<uint32_t>(tilesetSource.tilesetFilename);

		if (tilesetSource.tilesetFilename.size() > 8) {
			throw std::runtime_error("Tileset name may not be greater than 8 characters in length.");
		}

		if (tilesetSource.tilesetFilename.size() > 0) {
			streamReader.Read(tilesetSource.numTiles);
		}
	}
}

void MapData::ReadVersionTag(Stream::Reader& streamReader)
{
	uint32_t versionTag;
	streamReader.Read(versionTag);

	if (versionTag < 0x1010)
	{
		std::cerr << "All instances of version tag in .map and .op2 files should be greater than 0x1010.";
	}
}

void MapData::ReadTileGroups(Stream::Reader& streamReader, MapData& mapData)
{
	uint32_t numTileGroups;
	streamReader.Read(numTileGroups);
	uint32_t unknown;
	streamReader.Read(unknown); // Read unknown/unused field (skip past it)

	for (uint32_t i = 0; i < numTileGroups; ++i)
	{
		mapData.tileGroups.push_back(ReadTileGroup(streamReader));
	}
}

TileGroup MapData::ReadTileGroup(Stream::Reader& streamReader)
{
	TileGroup tileGroup;

	streamReader.Read(tileGroup.tileWidth);
	streamReader.Read(tileGroup.tileHeight);

	tileGroup.mappingIndices.resize(tileGroup.tileWidth * tileGroup.tileHeight);
	streamReader.Read(tileGroup.mappingIndices);

	streamReader.Read<uint32_t>(tileGroup.name);

	return tileGroup;
}
