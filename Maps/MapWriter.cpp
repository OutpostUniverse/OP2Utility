#include "MapWriter.h"
#include "../StreamWriter.h"

void MapWriter::Write(SeekableStreamWriter& mapStream, const MapData& mapData)
{	
	streamWriter = &mapStream;

	WriteHeader(mapData.header);
	WriteTiles(mapData.tiles);
	WriteClipRect(mapData.clipRect);
	WriteTilesetSources(mapData.tilesetSources);
	WriteTilesetHeader();
	WriteTileInfo(mapData.tileInfos);
	WriteTerrainType(mapData.terrainTypes);

	WriteVersionTag(mapData.header.versionTag);
	WriteVersionTag(mapData.header.versionTag);

	WriteTileGroups(mapData.tileGroups);
}

// Creates a new map file by truncating an existing file if it exists.
void MapWriter::Write(const std::string& filename, const MapData& mapData)
{
	Write(FileStreamWriter(filename, FileWriteFlag::Overwrite | FileWriteFlag::Truncate), mapData);
}

void MapWriter::WriteHeader(const MapHeader& header)
{
	if (!header.VersionTagValid())
	{
		throw std::runtime_error("All instances of version tag in .map and .op2 files must be greater than 0x1010.");
	}

	streamWriter->Write((char*)&header, sizeof(header));
}

void MapWriter::WriteVersionTag(int versionTag)
{
	streamWriter->Write((char*)&versionTag, sizeof(versionTag));
}

void MapWriter::WriteTiles(const std::vector<TileData>& tiles)
{
	streamWriter->Write((char*)&tiles[0], tiles.size() * sizeof(TileData));
}

void MapWriter::WriteClipRect(const ClipRect& clipRect)
{
	streamWriter->Write((char*)&clipRect, sizeof(clipRect));
}

void MapWriter::WriteTilesetSources(const std::vector<TilesetSource>& tileSetSources)
{
	for (TilesetSource tilesetSource : tileSetSources)
	{
		WriteString(tilesetSource.tilesetFilename);

		// Only include the number of tiles if the tileset contains a filename.
		if (tilesetSource.tilesetFilename.size() > 0)
		{
			streamWriter->Write((char*)&tilesetSource.numTiles, sizeof(int));
		}
	}
}

void MapWriter::WriteTilesetHeader()
{
	streamWriter->Write("TILE SET\x1a", 10);
}

void MapWriter::WriteTileInfo(const std::vector<TileInfo>& tileInfos)
{
	WriteContainerSize(tileInfos.size());

	for (TileInfo tileInfo : tileInfos)
	{
		streamWriter->Write((char*)&tileInfo, sizeof(TileInfo));
	}
}

void MapWriter::WriteTerrainType(const std::vector<TerrainType>& terrainTypes)
{
	WriteContainerSize(terrainTypes.size());

	for (TerrainType terrainType : terrainTypes)
	{
		streamWriter->Write((char*)&terrainType, sizeof(TerrainType));
	}
}

void MapWriter::WriteTileGroups(const std::vector<TileGroup>& tileGroups)
{
	WriteContainerSize(tileGroups.size());

	streamWriter->Seek(sizeof(int));

	for (TileGroup tileGroup : tileGroups)
	{
		streamWriter->Write((char*)&tileGroup.tileWidth, sizeof(tileGroup.tileWidth));
		streamWriter->Write((char*)&tileGroup.tileHeight, sizeof(tileGroup.tileHeight));

		for (int mappingIndex : tileGroup.mappingIndices)
		{
			streamWriter->Write((char*)&mappingIndex, sizeof(mappingIndex));
		}

		WriteString(tileGroup.name);
	}
}

void MapWriter::WriteContainerSize(size_t size)
{
	streamWriter->Write((char*)&size, sizeof(size));
}

// String must be stored in file as string length followed by char[].
void MapWriter::WriteString(const std::string& s)
{
	WriteContainerSize(s.size());

	if (s.size() > 0) {
		streamWriter->Write(s.c_str(), s.size());
	}
}
