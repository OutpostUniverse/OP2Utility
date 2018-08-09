#include "MapWriter.h"
#include "../Streams/FileStreamWriter.h"
#include <cstdint>
#include <stdexcept>
#include <vector>


namespace MapWriter {
	// Anonymous namespace to hold private methods
	namespace {
		void WriteHeader(StreamWriter& streamWriter, const MapHeader& header);
		void WriteTilesetSources(StreamWriter& streamWriter, const std::vector<TilesetSource>& tilesetSources);
		void WriteTileInfo(StreamWriter& streamWriter, const std::vector<TileInfo>& tileInfos);
		void WriteTerrainType(StreamWriter& streamWriter, const std::vector<TerrainType>& terrainTypes);
		void WriteTileGroups(StreamWriter& streamWriter, const std::vector<TileGroup>& tileGroups);
		void WriteContainerSize(StreamWriter& streamWriter, uint32_t size);
	}


	// ==== Public methods ====


	void Write(const std::string& filename, const MapData& mapData)
	{
		FileStreamWriter mapWriter(filename);
		Write(mapWriter, mapData);
	}

	void Write(StreamWriter& streamWriter, const MapData& mapData)
	{
		WriteHeader(streamWriter, mapData.header);
		streamWriter.Write(&mapData.tiles[0], mapData.tiles.size() * sizeof(TileData));
		streamWriter.Write(mapData.clipRect);
		WriteTilesetSources(streamWriter, mapData.tilesetSources);
		streamWriter.Write("TILE SET\x1a", 10);
		WriteTileInfo(streamWriter, mapData.tileInfos);
		WriteTerrainType(streamWriter, mapData.terrainTypes);

		streamWriter.Write(mapData.header.versionTag);
		streamWriter.Write(mapData.header.versionTag);

		WriteTileGroups(streamWriter, mapData.tileGroups);
	}


	// == Private methods ==


	namespace {
		void WriteHeader(StreamWriter& streamWriter, const MapHeader& header)
		{
			if (!header.VersionTagValid()) {
				throw std::runtime_error("All instances of version tag in .map and .op2 files must be greater than 0x1010.");
			}

			streamWriter.Write(header);
		}

		void WriteTilesetSources(StreamWriter& streamWriter, const std::vector<TilesetSource>& tilesetSources)
		{
			for (const auto& tilesetSource : tilesetSources)
			{
				streamWriter.Write<uint32_t>(tilesetSource.tilesetFilename);

				// Only include the number of tiles if the tileset contains a filename.
				if (tilesetSource.tilesetFilename.size() > 0)
				{
					streamWriter.Write(tilesetSource.numTiles);
				}
			}
		}

		void WriteTileInfo(StreamWriter& streamWriter, const std::vector<TileInfo>& tileInfos)
		{
			WriteContainerSize(streamWriter, tileInfos.size());

			streamWriter.Write(tileInfos);
		}

		void WriteTerrainType(StreamWriter& streamWriter, const std::vector<TerrainType>& terrainTypes)
		{
			WriteContainerSize(streamWriter, terrainTypes.size());

			streamWriter.Write(terrainTypes);
		}

		void WriteTileGroups(StreamWriter& streamWriter, const std::vector<TileGroup>& tileGroups)
		{
			WriteContainerSize(streamWriter, tileGroups.size());

			uint32_t unknown = tileGroups.size() - 1; // Write unknown field with best guess as to what value it should hold
			streamWriter.Write(unknown);

			for (const auto& tileGroup : tileGroups)
			{
				streamWriter.Write(tileGroup.tileWidth);
				streamWriter.Write(tileGroup.tileHeight);

				streamWriter.Write(tileGroup.mappingIndices);

				streamWriter.Write<uint32_t>(tileGroup.name);
			}
		}

		// Outpost 2 map files represent container sizes as 4 byte values
		void WriteContainerSize(StreamWriter& streamWriter, uint32_t size)
		{
			streamWriter.Write(size);
		}
	}
}
