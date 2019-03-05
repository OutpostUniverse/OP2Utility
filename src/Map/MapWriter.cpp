#include "Map.h"
#include "MapHeader.h"
#include "../Stream/FileWriter.h"
#include "../BitTwiddle.h"
#include <stdexcept>
#include <cmath>
#include <limits>

void Map::Write(const std::string& filename) const
{
	Stream::FileWriter mapStream(filename);
	this->Write(mapStream);
}

void Map::Write(Stream::Writer& mapStream) const
{
	const Map& map = *this;
	MapHeader mapHeader = map.CreateHeader();

	mapStream.Write(mapHeader);
	mapStream.Write(map.tiles);
	mapStream.Write(map.clipRect);
	WriteTilesetSources(mapStream, map.tilesetSources);
	mapStream.Write("TILE SET\x1a", 10);
	mapStream.Write<uint32_t>(map.tileMappings);
	mapStream.Write<uint32_t>(map.terrainTypes);

	mapStream.Write(mapHeader.versionTag);
	mapStream.Write(mapHeader.versionTag);

	WriteTileGroups(mapStream, map.tileGroups);
}


// == Private methods ==

MapHeader Map::CreateHeader() const
{
	MapHeader mapHeader;

	mapHeader.versionTag = versionTag;
	mapHeader.bSavedGame = isSavedGame;
	mapHeader.lgWidthInTiles = GetWidthInTilesLog2(widthInTiles);
	mapHeader.heightInTiles = heightInTiles;

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

void Map::WriteTilesetSources(Stream::Writer& stream, const std::vector<TilesetSource>& tilesetSources)
{
	for (const auto& tilesetSource : tilesetSources)
	{
		stream.Write<uint32_t>(tilesetSource.tilesetFilename);

		// Only include the number of tiles if the tileset contains a filename.
		if (tilesetSource.tilesetFilename.size() > 0)
		{
			stream.Write(tilesetSource.numTiles);
		}
	}
}

void Map::WriteTileGroups(Stream::Writer& stream, const std::vector<TileGroup>& tileGroups)
{
	WriteContainerSize(stream, tileGroups.size());
	// tileGroups.size is checked to ensure it is below UINT32_MAX by previous call to WriteContainerSize.
	// Write unknown field with best guess as to what value it should hold
	uint32_t unknown = !tileGroups.empty() ? static_cast<uint32_t>(tileGroups.size()) - 1 : 0;
	stream.Write(unknown);

	for (const auto& tileGroup : tileGroups)
	{
		stream.Write(tileGroup.tileWidth);
		stream.Write(tileGroup.tileHeight);

		stream.Write(tileGroup.mappingIndices);

		stream.Write<uint32_t>(tileGroup.name);
	}
}

// Outpost 2 map files represent container sizes as 4 byte values
void Map::WriteContainerSize(Stream::Writer& stream, std::size_t size)
{
	if (size > UINT32_MAX) {
		throw std::runtime_error("Container size is too large for writing into an Outpost 2 map");
	}

	stream.Write(static_cast<uint32_t>(size));
}
	