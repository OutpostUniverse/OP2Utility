#include "Map.h"
#include "SavedGameDataSection2.h"
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
	Map map = ReadMapBeginning(streamReader);

	ReadVersionTag(streamReader, map.versionTag);
	ReadVersionTag(streamReader, map.versionTag);

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

	Map map = ReadMapBeginning(streamReader);
	
	ReadVersionTag(streamReader, map.versionTag);

	ReadSavedGameSection2(streamReader);
	
	ReadVersionTag(streamReader, map.versionTag);

	// TODO: Read data after final version tag.

	return map;
}


// == Private methods ==

void Map::SkipSaveGameHeader(Stream::SeekableReader& streamReader)
{
	streamReader.SeekRelative(0x1E025);
}

Map Map::ReadMapBeginning(Stream::Reader& streamReader)
{
	MapHeader mapHeader;
	streamReader.Read(mapHeader);
	CheckMinVersionTag(mapHeader.versionTag);

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

	return map;
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

void Map::ReadVersionTag(Stream::Reader& streamReader, uint32_t lastVersionTag)
{
	uint32_t nextVersionTag;
	streamReader.Read(nextVersionTag);

	CheckMinVersionTag(nextVersionTag);

	if (nextVersionTag != lastVersionTag) {
		throw std::runtime_error("Mismatched version tags detected. Version tag 1: " + 
			std::to_string(lastVersionTag) + ". Version tag 2: " + std::to_string(nextVersionTag));
	}
}

void Map::ReadSavedGameSection2(Stream::SeekableReader& streamReader)
{
	SavedGameDataSection2 savedGameData;

	streamReader.Read(savedGameData.unitCount);
	streamReader.Read(savedGameData.unknown1);
	streamReader.Read(savedGameData.unknown2);
	streamReader.Read(savedGameData.unknown3);
	streamReader.Read(savedGameData.sizeOfUnit);

	streamReader.Read(savedGameData.objectCount1);
	streamReader.Read(savedGameData.objectCount2);

	savedGameData.objects1.resize(512 * savedGameData.objectCount1);
	streamReader.Read(savedGameData.objects1);
	savedGameData.objects2.resize(savedGameData.objectCount2);
	streamReader.Read(savedGameData.objects2);

	streamReader.Read(savedGameData.unitID1);
	streamReader.Read(savedGameData.unitID2);

	//streamReader.Read(savedGameData.unitRecord);
	streamReader.SeekRelative(0x3BF88);
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
