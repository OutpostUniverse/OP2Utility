#include "MapWriter.h"
#include <stdexcept>

void MapWriter::Write(const std::string& filename, const MapData& mapData)
{
	fileStream.open(filename.c_str(), std::ios::trunc | std::ios::out | std::ios::binary);

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

	fileStream.close();
}

void MapWriter::WriteHeader(const MapHeader& header)
{
	if (!header.VersionTagValid())
	{
		throw std::runtime_error("All instances of version tag in .map and .op2 files must be greater than 0x1010.");
	}

	fileStream.write((char*)&header, sizeof(header));
}

void MapWriter::WriteVersionTag(int versionTag)
{
	fileStream.write((char*)&versionTag, sizeof(versionTag));
}

void MapWriter::WriteTiles(const std::vector<TileData>& tiles)
{
	fileStream.write((char*)&tiles[0], tiles.size() * sizeof(TileData));
}

void MapWriter::WriteClipRect(const ClipRect& clipRect)
{
	fileStream.write((char*)&clipRect, sizeof(clipRect));
}

void MapWriter::WriteTilesetSources(const std::vector<TilesetSource>& tileSetSources)
{
	for (TilesetSource tilesetSource : tileSetSources)
	{
		WriteString(tilesetSource.tilesetFilename);

		// Only include the number of tiles if the tileset contains a filename.
		if (tilesetSource.tilesetFilename.size() > 0)
		{
			fileStream.write((char*)&tilesetSource.numTiles, sizeof(int));
		}
	}
}

void MapWriter::WriteTilesetHeader()
{
	fileStream.write("TILE SET\x1a", 10);
}

void MapWriter::WriteTileInfo(const std::vector<TileInfo>& tileInfos)
{
	WriteContainerSize(tileInfos.size());

	for (TileInfo tileInfo : tileInfos)
	{
		fileStream.write((char*)&tileInfo, sizeof(TileInfo));
	}
}

void MapWriter::WriteTerrainType(const std::vector<TerrainType>& terrainTypes)
{
	WriteContainerSize(terrainTypes.size());

	for (TerrainType terrainType : terrainTypes)
	{
		fileStream.write((char*)&terrainType, sizeof(TerrainType));
	}
}

void MapWriter::WriteTileGroups(const std::vector<TileGroup>& tileGroups)
{
	WriteContainerSize(tileGroups.size());

	int temp = 0;
	fileStream.write((char*)&temp, sizeof(temp));

	for (TileGroup tileGroup : tileGroups)
	{
		fileStream.write((char*)&tileGroup.tileWidth, sizeof(tileGroup.tileWidth));
		fileStream.write((char*)&tileGroup.tileHeight, sizeof(tileGroup.tileHeight));

		for (int mappingIndex : tileGroup.mappingIndices)
		{
			fileStream.write((char*)&mappingIndex, sizeof(mappingIndex));
		}

		WriteString(tileGroup.name);
	}
}

void MapWriter::WriteContainerSize(size_t size)
{
	fileStream.write((char*)&size, sizeof(size));
}

// String must be stored in file as string length followed by char[].
void MapWriter::WriteString(const std::string& s)
{
	WriteContainerSize(s.size());

	if (s.size() > 0) {
		fileStream.write(s.c_str(), s.size());
	}
}
