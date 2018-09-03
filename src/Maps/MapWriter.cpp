#include "MapWriter.h"
#include "../Streams/FileWriter.h"
#include <cstdint>
#include <cstddef>
#include <stdexcept>
#include <vector>


namespace MapWriter {
	// Anonymous namespace to hold private methods
	namespace {
		void ValidateMap(const MapData& mapData);
		void WriteTilesetSources(Stream::Writer& streamWriter, const std::vector<TilesetSource>& tilesetSources);
		void WriteTileGroups(Stream::Writer& streamWriter, const std::vector<TileGroup>& tileGroups);
		void WriteContainerSize(Stream::Writer& streamWriter, std::size_t size);
	}


	// ==== Public methods ====


	void Write(const std::string& filename, const MapData& mapData)
	{
		Stream::FileWriter mapWriter(filename);
		Write(mapWriter, mapData);
	}

	void Write(Stream::Writer& streamWriter, const MapData& mapData)
	{
		ValidateMap(mapData);

		streamWriter.Write(mapData.header);
		streamWriter.Write(mapData.tiles);
		streamWriter.Write(mapData.clipRect);
		WriteTilesetSources(streamWriter, mapData.tilesetSources);
		streamWriter.Write("TILE SET\x1a", 10);
		streamWriter.Write<uint32_t>(mapData.tileInfos);
		streamWriter.Write<uint32_t>(mapData.terrainTypes);

		streamWriter.Write(mapData.header.versionTag);
		streamWriter.Write(mapData.header.versionTag);

		WriteTileGroups(streamWriter, mapData.tileGroups);
	}


	// == Private methods ==


	namespace {
		void ValidateMap(const MapData& mapData) {
			if (mapData.header.TileCount() != mapData.tiles.size()) {
				throw std::runtime_error("Header reported tile width * tile height does not match actual tile count");
			}

			if (mapData.header.tilesetCount != mapData.tilesetSources.size()) {
				throw std::runtime_error("Header reported tileset count does not match actual tileset sources");
			}
		}

		void WriteTilesetSources(Stream::Writer& streamWriter, const std::vector<TilesetSource>& tilesetSources)
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

		void WriteTileGroups(Stream::Writer& streamWriter, const std::vector<TileGroup>& tileGroups)
		{
			WriteContainerSize(streamWriter, tileGroups.size());
			// tileGroups.size is checked to ensure it is below UINT32_MAX by previous call to WriteContainerSize.
			// Write unknown field with best guess as to what value it should hold
			uint32_t unknown = !tileGroups.empty() ? static_cast<uint32_t>(tileGroups.size()) - 1 : 0;
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
		void WriteContainerSize(Stream::Writer& streamWriter, std::size_t size)
		{
			if (size > UINT32_MAX) {
				throw std::runtime_error("Container size is too large for writing into an Outpost 2 map");
			}

			streamWriter.Write(static_cast<uint32_t>(size));
		}
	}
}
