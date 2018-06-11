#include "MapReader.h"
#include "../Streams/FileStreamReader.h"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstring>


namespace MapReader {
	// Anonymous namespace to hold private methods
	namespace {
		void SkipSaveGameHeader(SeekableStreamReader& streamReader);
		void ReadHeader(StreamReader& streamReader, MapData& mapData);
		void ReadTiles(StreamReader& streamReader, MapData& mapData);
		void ReadClipRect(StreamReader& streamReader, ClipRect& clipRect);
		void ReadTilesetSources(StreamReader& streamReader, MapData& mapData);
		void ReadTilesetHeader(StreamReader& streamReader);
		void ReadTileInfo(StreamReader& streamReader, MapData& mapData);
		void ReadVersionTag(StreamReader& streamReader);
		void ReadTileGroups(SeekableStreamReader& streamReader, MapData& mapData);
		TileGroup ReadTileGroup(StreamReader& streamReader);
		std::string ReadString(StreamReader& streamReader);
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

		ReadHeader(streamReader, mapData);
		ReadTiles(streamReader, mapData);
		ReadClipRect(streamReader, mapData.clipRect);
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

		void ReadHeader(StreamReader& streamReader, MapData& mapData)
		{
			streamReader.Read(&mapData.header, sizeof(mapData.header));
		}

		void ReadTiles(StreamReader& streamReader, MapData& mapData)
		{
			mapData.tiles.resize(mapData.header.TileCount());
			streamReader.Read(&mapData.tiles[0], mapData.tiles.size() * sizeof(TileData));
		}

		void ReadClipRect(StreamReader& streamReader, ClipRect& clipRect)
		{
			streamReader.Read(&clipRect, sizeof(clipRect));
		}

		void ReadTilesetHeader(StreamReader& streamReader)
		{
			char buffer[10];
			streamReader.Read(buffer, sizeof(buffer));

			if (std::strncmp(buffer, "TILE SET\x1a", sizeof(buffer))) {
				throw std::runtime_error("'TILE SET' string not found.");
			}
		}

		void ReadTilesetSources(StreamReader& streamReader, MapData& mapData)
		{
			mapData.tilesetSources.resize(static_cast<size_t>(mapData.header.numTilesets));

			for (unsigned int i = 0; i < mapData.header.numTilesets; ++i)
			{
				mapData.tilesetSources[i].tilesetFilename = ReadString(streamReader);

				if (mapData.tilesetSources[i].tilesetFilename.size() > 8) {
					throw std::runtime_error("Tileset name may not be greater than 8 characters in length.");
				}

				if (mapData.tilesetSources[i].tilesetFilename.size() > 0) {
					streamReader.Read(&mapData.tilesetSources[i].numTiles, sizeof(int));
				}
			}
		}

		void ReadTileInfo(StreamReader& streamReader, MapData& mapData)
		{
			size_t numTileInfo;
			streamReader.Read(&numTileInfo, sizeof(numTileInfo));

			mapData.tileInfos.resize(numTileInfo);
			streamReader.Read(&mapData.tileInfos[0], numTileInfo * sizeof(TileInfo));

			size_t numTerrainTypes;
			streamReader.Read(&numTerrainTypes, sizeof(numTerrainTypes));
			mapData.terrainTypes.resize(numTerrainTypes);
			streamReader.Read(&mapData.terrainTypes[0], numTerrainTypes * sizeof(TerrainType));
		}

		void ReadVersionTag(StreamReader& streamReader)
		{
			int versionTag;
			streamReader.Read(&versionTag, sizeof(versionTag));

			if (versionTag < 0x1010)
			{
				std::cerr << "All instances of version tag in .map and .op2 files should be greater than 0x1010.";
			}
		}

		void ReadTileGroups(SeekableStreamReader& streamReader, MapData& mapData)
		{
			int numTileGroups;
			streamReader.Read(&numTileGroups, sizeof(numTileGroups));
			streamReader.SeekRelative(4);

			for (int i = 0; i < numTileGroups; ++i)
			{
				mapData.tileGroups.push_back(ReadTileGroup(streamReader));
			}
		}

		TileGroup ReadTileGroup(StreamReader& streamReader)
		{
			TileGroup tileGroup;

			streamReader.Read(&tileGroup.tileWidth, sizeof(tileGroup.tileWidth));
			streamReader.Read(&tileGroup.tileHeight, sizeof(tileGroup.tileHeight));

			int mappingIndex;
			for (int i = 0; i < tileGroup.tileHeight * tileGroup.tileWidth; ++i)
			{
				streamReader.Read(&mappingIndex, sizeof(mappingIndex));
				tileGroup.mappingIndices.push_back(mappingIndex);
			}

			tileGroup.name = ReadString(streamReader);

			return tileGroup;
		}

		// String must be stored in file as string length followed by char[].
		std::string ReadString(StreamReader& streamReader)
		{
			uint32_t stringLength;
			streamReader.Read(&stringLength, sizeof(stringLength));

			std::string s;
			s.resize(stringLength);
			streamReader.Read(&s[0], stringLength);

			return s;
		}
	}
}
