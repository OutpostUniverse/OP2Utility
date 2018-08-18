#include "MapWriter.h"
#include "../Streams/FileStreamWriter.h"
#include <cstdint>
#include <cstddef>
#include <stdexcept>
#include <vector>


namespace MapWriter {
	// Anonymous namespace to hold private methods
	namespace {
		void WriteHeader(Stream::StreamWriter& streamWriter, const MapHeader& header);
		void WriteTilesetSources(Stream::StreamWriter& streamWriter, const std::vector<TilesetSource>& tilesetSources);
		void WriteTileGroups(Stream::StreamWriter& streamWriter, const std::vector<TileGroup>& tileGroups);
		void WriteContainerSize(Stream::StreamWriter& streamWriter, std::size_t size);
	}


	// ==== Public methods ====


	void Write(const std::string& filename, const MapData& mapData)
	{
		Stream::FileStreamWriter mapWriter(filename);
		Write(mapWriter, mapData);
	}

	void Write(Stream::StreamWriter& streamWriter, const MapData& mapData)
	{
		WriteHeader(streamWriter, mapData.header);
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
		void WriteHeader(Stream::StreamWriter& streamWriter, const MapHeader& header)
		{
			if (!header.VersionTagValid()) {
				throw std::runtime_error("All instances of version tag in .map and .op2 files must be greater than 0x1010.");
			}

			streamWriter.Write(header);
		}

		void WriteTilesetSources(Stream::StreamWriter& streamWriter, const std::vector<TilesetSource>& tilesetSources)
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

		void WriteTileGroups(Stream::StreamWriter& streamWriter, const std::vector<TileGroup>& tileGroups)
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
		void WriteContainerSize(Stream::StreamWriter& streamWriter, std::size_t size)
		{
			if (size > UINT32_MAX) {
				throw std::runtime_error("Container size is too large for writing into an Outpost 2 maps.");
			}

			streamWriter.Write(static_cast<uint32_t>(size));
		}
	}
}
