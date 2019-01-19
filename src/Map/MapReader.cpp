#include "Map.h"
#include "MapHeader.h"
#include "../Stream/FileReader.h"
#include <iostream>
#include <stdexcept>
#include <array>
#include <cstring>

const std::array<char, 10> tilesetHeader{ "TILE SET\x1a" };

Map Map::ReadMap(std::string filename)
{
	Stream::FileReader mapReader(filename);
	return ReadMap(mapReader);
}

Map Map::ReadMap(Stream::Reader& streamReader)
{
	MapHeader mapHeader;
	streamReader.Read(mapHeader);

	Map map;
	map.versionTag = mapHeader.versionTag;
	map.isSavedGame = mapHeader.bSavedGame;
	map.mapTileWidth = mapHeader.MapTileWidth();
	map.mapTileHeight = mapHeader.mapTileHeight;

	map.tiles.resize(mapHeader.TileCount());
	streamReader.Read(map.tiles);

	streamReader.Read(map.clipRect);
	ReadTilesetSources(streamReader, map, static_cast<std::size_t>(mapHeader.tilesetCount));
	ReadTilesetHeader(streamReader);
	streamReader.Read<uint32_t>(map.tileInfos);
	streamReader.Read<uint32_t>(map.terrainTypes);
	ReadVersionTag(streamReader);
	ReadVersionTag(streamReader);
	ReadTileGroups(streamReader, map);

	return map;
}

Map Map::ReadSavedGame(std::string filename)
{
	Stream::FileReader mapReader(filename);
	return ReadSavedGame(mapReader);
}

Map Map::ReadSavedGame(Stream::SeekableReader& streamReader)
{
	SkipSaveGameHeader(streamReader);
	return ReadMap(streamReader);
}


// == Private methods ==

void Map::SkipSaveGameHeader(Stream::SeekableReader& streamReader)
{
	streamReader.SeekForward(0x1E025);
}

void Map::ReadTilesetHeader(Stream::Reader& streamReader)
{
	std::array<char, 10> buffer;
	streamReader.Read(buffer);

	if (buffer != tilesetHeader) {
		throw std::runtime_error("'TILE SET' string not found.");
	}
}

void Map::ReadTilesetSources(Stream::Reader& streamReader, Map& map, std::size_t tilesetCount)
{
	map.tilesetSources.resize(tilesetCount);

	for (auto& tilesetSource : map.tilesetSources)
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

void Map::ReadVersionTag(Stream::Reader& streamReader)
{
	uint32_t versionTag;
	streamReader.Read(versionTag);

	if (versionTag < MapHeader::MinMapVersion)
	{
		throw std::runtime_error(
			"All instances of version tag in .map and .op2 files should be greater than " +
			std::to_string(MapHeader::MinMapVersion) + ".\n" +
			"Found version tag is " + std::to_string(versionTag) + "."
		);
	}
}

void Map::ReadTileGroups(Stream::Reader& streamReader, Map& map)
{
	uint32_t numTileGroups;
	streamReader.Read(numTileGroups);
	uint32_t unknown;
	streamReader.Read(unknown); // Read unknown/unused field (skip past it)

	for (uint32_t i = 0; i < numTileGroups; ++i)
	{
		map.tileGroups.push_back(ReadTileGroup(streamReader));
	}
}

TileGroup Map::ReadTileGroup(Stream::Reader& streamReader)
{
	TileGroup tileGroup;

	streamReader.Read(tileGroup.tileWidth);
	streamReader.Read(tileGroup.tileHeight);

	tileGroup.mappingIndices.resize(tileGroup.tileWidth * tileGroup.tileHeight);
	streamReader.Read(tileGroup.mappingIndices);

	streamReader.Read<uint32_t>(tileGroup.name);

	return tileGroup;
}
