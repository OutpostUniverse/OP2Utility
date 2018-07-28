#include "MapReader.h"
#include "../Streams/FileStreamReader.h"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <array>
#include <cstring>
#include <cstdint>


namespace MapReader {
	// Anonymous namespace to hold private methods
	namespace {
		void SkipSaveGameHeader(SeekableStreamReader& streamReader);
		void ReadTiles(StreamReader& streamReader, MapData& mapData);
		void ReadTilesetSources(StreamReader& streamReader, MapData& mapData);
		void ReadTilesetHeader(StreamReader& streamReader);
		void ReadTileInfo(StreamReader& streamReader, MapData& mapData);
		void ReadVersionTag(StreamReader& streamReader);
		void ReadTileGroups(StreamReader& streamReader, MapData& mapData);
		TileGroup ReadTileGroup(StreamReader& streamReader);
		std::string ReadString(StreamReader& streamReader);

		const std::array<char, 10> tilesetHeader{ "TILE SET\x1a" };
	}


	// ==== Public methohds ====


	MapData Read(std::string filename, bool savedGame)
	{
		FileStreamReader mapReader(filename);
		return Read(mapReader, savedGame);
	}

	MapData Read(SeekableStreamReader& streamReader, bool savedGame)
	{
		MapData mapData;

		if (savedGame) {
			SkipSaveGameHeader(streamReader);
		}

		streamReader.Read(mapData.header);
		ReadTiles(streamReader, mapData);
		streamReader.Read(mapData.clipRect);
		ReadTilesetSources(streamReader, mapData);
		ReadTilesetHeader(streamReader);
		ReadTileInfo(streamReader, mapData);
		ReadVersionTag(streamReader);
		ReadVersionTag(streamReader);
		ReadTileGroups(streamReader, mapData);

		return mapData;
	}


	// == Private methods ==


	namespace {
		void SkipSaveGameHeader(SeekableStreamReader& streamReader)
		{
			streamReader.SeekRelative(0x1E025);
		}

		void ReadTiles(StreamReader& streamReader, MapData& mapData)
		{
			mapData.tiles.resize(mapData.header.TileCount());
			streamReader.Read(&mapData.tiles[0], mapData.tiles.size() * sizeof(TileData));
		}

		void ReadTilesetHeader(StreamReader& streamReader)
		{
			std::array<char, 10> buffer;
			streamReader.Read(buffer);

			if (buffer != tilesetHeader) {
				throw std::runtime_error("'TILE SET' string not found.");
			}
		}

		void ReadTilesetSources(StreamReader& streamReader, MapData& mapData)
		{
			mapData.tilesetSources.resize(static_cast<std::size_t>(mapData.header.numTilesets));

			for (auto& tilesetSource : mapData.tilesetSources)
			{
				tilesetSource.tilesetFilename = ReadString(streamReader);

				if (tilesetSource.tilesetFilename.size() > 8) {
					throw std::runtime_error("Tileset name may not be greater than 8 characters in length.");
				}

				if (tilesetSource.tilesetFilename.size() > 0) {
					streamReader.Read(tilesetSource.numTiles);
				}
			}
		}

		void ReadTileInfo(StreamReader& streamReader, MapData& mapData)
		{
			uint32_t numTileInfo;
			streamReader.Read(numTileInfo);

			mapData.tileInfos.resize(numTileInfo);
			streamReader.Read(&mapData.tileInfos[0], numTileInfo * sizeof(TileInfo));

			std::size_t numTerrainTypes;
			streamReader.Read(numTerrainTypes);
			mapData.terrainTypes.resize(numTerrainTypes);
			streamReader.Read(&mapData.terrainTypes[0], numTerrainTypes * sizeof(TerrainType));
		}

		void ReadVersionTag(StreamReader& streamReader)
		{
			uint32_t versionTag;
			streamReader.Read(versionTag);

			if (versionTag < 0x1010)
			{
				std::cerr << "All instances of version tag in .map and .op2 files should be greater than 0x1010.";
			}
		}

		void ReadTileGroups(StreamReader& streamReader, MapData& mapData)
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

		TileGroup ReadTileGroup(StreamReader& streamReader)
		{
			TileGroup tileGroup;

			streamReader.Read(tileGroup.tileWidth);
			streamReader.Read(tileGroup.tileHeight);

			tileGroup.mappingIndices.resize(tileGroup.tileWidth * tileGroup.tileHeight);
			streamReader.Read(tileGroup.mappingIndices);

			tileGroup.name = ReadString(streamReader);

			return tileGroup;
		}

		// String must be stored in file as string length followed by char[].
		std::string ReadString(StreamReader& streamReader)
		{
			uint32_t stringLength;
			streamReader.Read(stringLength);

			std::string s;
			s.resize(stringLength);
			streamReader.Read(&s[0], stringLength);

			return s;
		}
	}
}
