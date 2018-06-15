#include "MapWriter.h"
#include "../Streams/FileStreamWriter.h"
#include <cstdint>
#include <stdexcept>
#include <vector>


namespace MapWriter {
	// Anonymous namespace to hold private methods
	namespace {
		void WriteHeader(StreamWriter& streamWriter, const MapHeader& header);
		void WriteTiles(StreamWriter& streamWriter, const std::vector<TileData>& tiles);
		void WriteClipRect(StreamWriter& streamWriter, const ClipRect& clipRect);
		void WriteTilesetSources(StreamWriter& streamWriter, const std::vector<TilesetSource>& tilesetSources);
		void WriteTilesetHeader(StreamWriter& streamWriter);
		void WriteTileInfo(StreamWriter& streamWriter, const std::vector<TileInfo>& tileInfos);
		void WriteTerrainType(StreamWriter& streamWriter, const std::vector<TerrainType>& terrainTypes);
		void WriteTileGroups(SeekableStreamWriter& streamWriter, const std::vector<TileGroup>& tileGroups);
		void WriteVersionTag(StreamWriter& streamWriter, int versionTag);
		void WriteContainerSize(StreamWriter& streamWriter, uint32_t size);
		void WriteString(StreamWriter& streamWriter, const std::string& s);
	}


	// ==== Public methods ====


	void Write(const std::string& filename, const MapData& mapData)
	{
		FileStreamWriter mapWriter(filename);
		Write(mapWriter, mapData);
	}

	void Write(SeekableStreamWriter& streamWriter, const MapData& mapData)
	{
		WriteHeader(streamWriter, mapData.header);
		WriteTiles(streamWriter, mapData.tiles);
		WriteClipRect(streamWriter, mapData.clipRect);
		WriteTilesetSources(streamWriter, mapData.tilesetSources);
		WriteTilesetHeader(streamWriter);
		WriteTileInfo(streamWriter, mapData.tileInfos);
		WriteTerrainType(streamWriter, mapData.terrainTypes);

		WriteVersionTag(streamWriter, mapData.header.versionTag);
		WriteVersionTag(streamWriter, mapData.header.versionTag);

		WriteTileGroups(streamWriter, mapData.tileGroups);
	}


	// == Private methods ==


	namespace {
		void WriteHeader(StreamWriter& streamWriter, const MapHeader& header)
		{
			if (!header.VersionTagValid())
			{
				throw std::runtime_error("All instances of version tag in .map and .op2 files must be greater than 0x1010.");
			}

			streamWriter.Write(&header, sizeof(header));
		}

		void WriteVersionTag(StreamWriter& streamWriter, int versionTag)
		{
			streamWriter.Write(&versionTag, sizeof(versionTag));
		}

		void WriteTiles(StreamWriter& streamWriter, const std::vector<TileData>& tiles)
		{
			streamWriter.Write(&tiles[0], tiles.size() * sizeof(TileData));
		}

		void WriteClipRect(StreamWriter& streamWriter, const ClipRect& clipRect)
		{
			streamWriter.Write(&clipRect, sizeof(clipRect));
		}

		void WriteTilesetSources(StreamWriter& streamWriter, const std::vector<TilesetSource>& tileSetSources)
		{
			for (TilesetSource tilesetSource : tileSetSources)
			{
				WriteString(streamWriter, tilesetSource.tilesetFilename);

				// Only include the number of tiles if the tileset contains a filename.
				if (tilesetSource.tilesetFilename.size() > 0)
				{
					streamWriter.Write(&tilesetSource.numTiles, sizeof(int));
				}
			}
		}

		void WriteTilesetHeader(StreamWriter& streamWriter)
		{
			streamWriter.Write("TILE SET\x1a", 10);
		}

		void WriteTileInfo(StreamWriter& streamWriter, const std::vector<TileInfo>& tileInfos)
		{
			WriteContainerSize(streamWriter, tileInfos.size());

			for (TileInfo tileInfo : tileInfos)
			{
				streamWriter.Write(&tileInfo, sizeof(TileInfo));
			}
		}

		void WriteTerrainType(StreamWriter& streamWriter, const std::vector<TerrainType>& terrainTypes)
		{
			WriteContainerSize(streamWriter, terrainTypes.size());

			for (TerrainType terrainType : terrainTypes)
			{
				streamWriter.Write(&terrainType, sizeof(TerrainType));
			}
		}

		void WriteTileGroups(SeekableStreamWriter& streamWriter, const std::vector<TileGroup>& tileGroups)
		{
			WriteContainerSize(streamWriter, tileGroups.size());

			streamWriter.SeekRelative(sizeof(int));

			for (TileGroup tileGroup : tileGroups)
			{
				streamWriter.Write(&tileGroup.tileWidth, sizeof(tileGroup.tileWidth));
				streamWriter.Write(&tileGroup.tileHeight, sizeof(tileGroup.tileHeight));

				for (int mappingIndex : tileGroup.mappingIndices)
				{
					streamWriter.Write(&mappingIndex, sizeof(mappingIndex));
				}

				WriteString(streamWriter, tileGroup.name);
			}
		}

		void WriteContainerSize(StreamWriter& streamWriter, uint32_t size)
		{
			streamWriter.Write(&size, sizeof(size));
		}

		// String must be stored in file as string length followed by char[].
		void WriteString(StreamWriter& streamWriter, const std::string& s)
		{
			WriteContainerSize(streamWriter, s.size());

			if (s.size() > 0) {
				streamWriter.Write(s.c_str(), s.size());
			}
		}
	}
}
