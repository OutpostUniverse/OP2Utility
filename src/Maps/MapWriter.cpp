#include "MapData.h"
#include "MapHeader.h"
#include "../Streams/FileWriter.h"
#include <stdexcept>
#include <cmath>
#include <limits>

void MapData::Write(const std::string& filename, const MapData& mapData)
{
	Stream::FileWriter mapWriter(filename);
	Write(mapWriter, mapData);
}

void MapData::Write(Stream::Writer& streamWriter, const MapData& mapData)
{
	MapHeader mapHeader = mapData.CreateHeader();

	streamWriter.Write(mapHeader);
	streamWriter.Write(mapData.tiles);
	streamWriter.Write(mapData.clipRect);
	WriteTilesetSources(streamWriter, mapData.tilesetSources);
	streamWriter.Write("TILE SET\x1a", 10);
	streamWriter.Write<uint32_t>(mapData.tileInfos);
	streamWriter.Write<uint32_t>(mapData.terrainTypes);

	streamWriter.Write(mapHeader.versionTag);
	streamWriter.Write(mapHeader.versionTag);

	WriteTileGroups(streamWriter, mapData.tileGroups);
}


// == Private methods ==

MapHeader MapData::CreateHeader() const
{
	MapHeader mapHeader;

	mapHeader.versionTag = versionTag;
	mapHeader.bSavedGame = isSavedGame;
	mapHeader.lgMapTileWidth = GetWidthInTilesLog2(mapTileWidth);
	mapHeader.mapTileHeight = mapTileHeight;

	if (tilesetSources.size() > std::numeric_limits<decltype(MapHeader::tilesetCount)>::max()) {
		throw std::runtime_error("Too many tilesets contained in map");
	}

	mapHeader.tilesetCount = static_cast<uint32_t>(tilesetSources.size());

	return mapHeader;
}

uint32_t MapData::GetWidthInTilesLog2(uint32_t widthInTiles) const
{
	auto lgWidthInTiles = std::log2(widthInTiles);

	double integerPart;
	if (std::modf(lgWidthInTiles, &integerPart) != 0.0) {
		throw std::runtime_error("Width of map in tiles must create an integer value when taking log2");
	}

	return static_cast<uint32_t>(lgWidthInTiles);
}

void MapData::WriteTilesetSources(Stream::Writer& streamWriter, const std::vector<TilesetSource>& tilesetSources)
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

void MapData::WriteTileGroups(Stream::Writer& streamWriter, const std::vector<TileGroup>& tileGroups)
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
void MapData::WriteContainerSize(Stream::Writer& streamWriter, std::size_t size)
{
	if (size > UINT32_MAX) {
		throw std::runtime_error("Container size is too large for writing into an Outpost 2 map");
	}

	streamWriter.Write(static_cast<uint32_t>(size));
}
	