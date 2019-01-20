#include "Map.h"
#include "MapHeader.h"
#include "../Stream/FileWriter.h"
#include "../BitTwiddle.h"
#include <stdexcept>
#include <cmath>
#include <limits>

void Map::Write(const std::string& filename) const
{
	Stream::FileWriter mapWriter(filename);
	this->Write(mapWriter);
}

void Map::Write(Stream::Writer& streamWriter) const
{
	const Map& map = *this;
	MapHeader mapHeader = map.CreateHeader();

	streamWriter.Write(mapHeader);
	streamWriter.Write(map.tiles);
	streamWriter.Write(map.clipRect);
	WriteTilesetSources(streamWriter, map.tilesetSources);
	streamWriter.Write("TILE SET\x1a", 10);
	streamWriter.Write<uint32_t>(map.tileMappings);
	streamWriter.Write<uint32_t>(map.terrainTypes);

	streamWriter.Write(mapHeader.versionTag);
	streamWriter.Write(mapHeader.versionTag);

	WriteTileGroups(streamWriter, map.tileGroups);
}


// == Private methods ==

MapHeader Map::CreateHeader() const
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

uint32_t Map::GetWidthInTilesLog2(uint32_t widthInTiles) const
{
	//if (!IsPowerOf2(widthInTiles)) {
	if (widthInTiles && !IsPowerOf2(widthInTiles)) {
		throw std::runtime_error("Map width in tiles must be a power of 2");
	}
	return Log2OfPowerOf2(widthInTiles);
}

void Map::WriteTilesetSources(Stream::Writer& streamWriter, const std::vector<TilesetSource>& tilesetSources)
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

void Map::WriteTileGroups(Stream::Writer& streamWriter, const std::vector<TileGroup>& tileGroups)
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
void Map::WriteContainerSize(Stream::Writer& streamWriter, std::size_t size)
{
	if (size > UINT32_MAX) {
		throw std::runtime_error("Container size is too large for writing into an Outpost 2 map");
	}

	streamWriter.Write(static_cast<uint32_t>(size));
}
	