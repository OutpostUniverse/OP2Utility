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
	Stream::FileReader mapStream(filename);
	return ReadMap(mapStream);
}

Map Map::ReadMap(Stream::Reader& mapStream)
{
	Map map = ReadMapBeginning(mapStream);

	ReadVersionTag(mapStream, map.versionTag);
	ReadVersionTag(mapStream, map.versionTag);

	ReadTileGroups(mapStream, map);

	return map;
}

Map Map::ReadSavedGame(std::string filename)
{
	Stream::FileReader savedGameStream(filename);
	return ReadSavedGame(savedGameStream);
}

Map Map::ReadSavedGame(Stream::BidirectionalSeekableReader& savedGameStream)
{
	SkipSaveGameHeader(savedGameStream);

	Map map = ReadMapBeginning(savedGameStream);
	
	ReadVersionTag(savedGameStream, map.versionTag);

	ReadSavedGameSection2(savedGameStream);
	
	ReadVersionTag(savedGameStream, map.versionTag);

	// TODO: Read data after final version tag.

	return map;
}


// == Private methods ==


void Map::SkipSaveGameHeader(Stream::BidirectionalSeekableReader& stream)
{
	stream.SeekForward(0x1E025);
}

Map Map::ReadMapBeginning(Stream::Reader& stream)
{
	MapHeader mapHeader;
	stream.Read(mapHeader);
	CheckMinVersionTag(mapHeader.versionTag);

	Map map;
	map.versionTag = mapHeader.versionTag;
	map.isSavedGame = mapHeader.bSavedGame;
	map.mapTileWidth = mapHeader.MapTileWidth();
	map.mapTileHeight = mapHeader.mapTileHeight;

	map.tiles.resize(mapHeader.TileCount());
	stream.Read(map.tiles);

	stream.Read(map.clipRect);
	ReadTilesetSources(stream, map, static_cast<std::size_t>(mapHeader.tilesetCount));
	ReadTilesetHeader(stream);
	stream.Read<uint32_t>(map.tileMappings);
	stream.Read<uint32_t>(map.terrainTypes);

	return map;
}

void Map::ReadTilesetHeader(Stream::Reader& stream)
{
	std::array<char, 10> buffer;
	stream.Read(buffer);

	if (buffer != tilesetHeader) {
		throw std::runtime_error("'TILE SET' string not found.");
	}
}

void Map::ReadTilesetSources(Stream::Reader& stream, Map& map, std::size_t tilesetCount)
{
	map.tilesetSources.resize(tilesetCount);

	for (auto& tilesetSource : map.tilesetSources)
	{
		stream.Read<uint32_t>(tilesetSource.tilesetFilename);

		if (tilesetSource.tilesetFilename.size() > 8) {
			throw std::runtime_error("Tileset name may not be greater than 8 characters in length.");
		}

		if (tilesetSource.tilesetFilename.size() > 0) {
			stream.Read(tilesetSource.numTiles);
		}
	}
}

void Map::ReadVersionTag(Stream::Reader& stream, uint32_t lastVersionTag)
{
	uint32_t nextVersionTag;
	stream.Read(nextVersionTag);

	CheckMinVersionTag(nextVersionTag);

	if (nextVersionTag != lastVersionTag) {
		throw std::runtime_error("Mismatched version tags detected. Version tag 1: " + 
			std::to_string(lastVersionTag) + ". Version tag 2: " + std::to_string(nextVersionTag));
	}
}

void Map::ReadSavedGameSection2(Stream::BidirectionalSeekableReader& stream)
{
	SavedGameDataSection2 savedGameData;

	stream.Read(savedGameData.unitCount);
	stream.Read(savedGameData.unknown1);
	stream.Read(savedGameData.unknown2);
	stream.Read(savedGameData.unknown3);
	stream.Read(savedGameData.sizeOfUnit);

	stream.Read(savedGameData.objectCount1);
	stream.Read(savedGameData.objectCount2);

	savedGameData.objects1.resize(savedGameData.objectCount1);
	stream.Read(savedGameData.objects1);
	savedGameData.objects2.resize(savedGameData.objectCount2);
	stream.Read(savedGameData.objects2);

	stream.Read(savedGameData.unitID1);
	stream.Read(savedGameData.unitID2);

	stream.Read(savedGameData.unitRecord);
}

void Map::ReadTileGroups(Stream::Reader& stream, Map& map)
{
	uint32_t numTileGroups;
	stream.Read(numTileGroups);
	uint32_t unknown;
	stream.Read(unknown); // Read unknown/unused field (skip past it)

	for (uint32_t i = 0; i < numTileGroups; ++i)
	{
		map.tileGroups.push_back(ReadTileGroup(stream));
	}
}

TileGroup Map::ReadTileGroup(Stream::Reader& stream)
{
	TileGroup tileGroup;

	stream.Read(tileGroup.tileWidth);
	stream.Read(tileGroup.tileHeight);

	tileGroup.mappingIndices.resize(tileGroup.tileWidth * tileGroup.tileHeight);
	stream.Read(tileGroup.mappingIndices);

	stream.Read<uint32_t>(tileGroup.name);

	return tileGroup;
}
