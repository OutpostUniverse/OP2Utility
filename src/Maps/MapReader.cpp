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
		void SkipSaveGameHeader(Stream::SeekableReader& streamReader);
		void ReadTiles(Stream::Reader& streamReader, MapData& mapData);
		void ReadTilesetSources(Stream::Reader& streamReader, MapData& mapData);
		void ReadTilesetHeader(Stream::Reader& streamReader);
		void ReadVersionTag(Stream::Reader& streamReader);
		void ReadTileGroups(Stream::Reader& streamReader, MapData& mapData);
		TileGroup ReadTileGroup(Stream::Reader& streamReader);

		const std::array<char, 10> tilesetHeader{ "TILE SET\x1a" };
	}


	// ==== Public methohds ====


	MapData Read(std::string filename, bool savedGame)
	{
		Stream::FileReader mapReader(filename);
		return Read(mapReader, savedGame);
	}

	MapData Read(Stream::SeekableReader& streamReader, bool savedGame)
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
		streamReader.Read<uint32_t>(mapData.tileInfos);
		streamReader.Read<uint32_t>(mapData.terrainTypes);
		ReadVersionTag(streamReader);
		ReadVersionTag(streamReader);
		ReadTileGroups(streamReader, mapData);

		return mapData;
	}


	// == Private methods ==


	namespace {
		void SkipSaveGameHeader(Stream::SeekableReader& streamReader)
		{
			streamReader.SeekRelative(0x1E025);
		}

		void ReadTiles(Stream::Reader& streamReader, MapData& mapData)
		{
			mapData.tiles.resize(mapData.header.TileCount());
			streamReader.Read(mapData.tiles);
		}

		void ReadTilesetHeader(Stream::Reader& streamReader)
		{
			std::array<char, 10> buffer;
			streamReader.Read(buffer);

			if (buffer != tilesetHeader) {
				throw std::runtime_error("'TILE SET' string not found.");
			}
		}

		void ReadTilesetSources(Stream::Reader& streamReader, MapData& mapData)
		{
			mapData.tilesetSources.resize(static_cast<std::size_t>(mapData.header.numTilesets));

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

		void ReadVersionTag(Stream::Reader& streamReader)
		{
			uint32_t versionTag;
			streamReader.Read(versionTag);

			if (versionTag < 0x1010)
			{
				std::cerr << "All instances of version tag in .map and .op2 files should be greater than 0x1010.";
			}
		}

		void ReadTileGroups(Stream::Reader& streamReader, MapData& mapData)
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

		TileGroup ReadTileGroup(Stream::Reader& streamReader)
		{
			TileGroup tileGroup;

			streamReader.Read(tileGroup.tileWidth);
			streamReader.Read(tileGroup.tileHeight);

			tileGroup.mappingIndices.resize(tileGroup.tileWidth * tileGroup.tileHeight);
			streamReader.Read(tileGroup.mappingIndices);

			streamReader.Read<uint32_t>(tileGroup.name);

			return tileGroup;
		}
	}
}
