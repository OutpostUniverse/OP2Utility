#pragma once

#include "MapData.h"
#include <string>
#include <vector>
#include <memory>

class SeekableStreamWriter;

// Writes an Outpost 2 map to file.
class MapWriter
{
public:
	void Write(const std::string& filename, const MapData& mapData);
	void Write(SeekableStreamWriter& mapStream, const MapData& mapData);

private:
	SeekableStreamWriter* streamWriter;

	void WriteHeader(const MapHeader& header);
	void WriteTiles(const std::vector<TileData>& tiles);
	void WriteClipRect(const ClipRect& clipRect);
	void WriteTilesetSources(const std::vector<TilesetSource>& tilesetSources);
	void WriteTilesetHeader();
	void WriteTileInfo(const std::vector<TileInfo>& tileInfos);
	void WriteTerrainType(const std::vector<TerrainType>& terrainTypes);
	void WriteTileGroups(const std::vector<TileGroup>& tileGroups);
	void WriteVersionTag(int versionTag);
	void WriteContainerSize(size_t size);
	void WriteString(const std::string& s);
};
