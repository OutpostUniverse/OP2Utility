#include "MapData.h"
#include "CellType.h"
#include <algorithm>
#include <cmath>
#include <limits>

MapData::MapData() :
	versionTag(minMapVersion),
	isSavedGame(false),
	mapTileWidth(0),
	mapTileHeight(0) { }

MapData::MapData(const MapHeader& mapHeader) :
	versionTag(mapHeader.versionTag),
	isSavedGame(mapHeader.bSavedGame),
	mapTileWidth(mapHeader.MapTileWidth()),
	mapTileHeight(mapHeader.mapTileHeight) { }

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

std::size_t MapData::GetTileInfoIndex(std::size_t x, std::size_t y) const
{
	return tiles[GetTileIndex(x, y)].tileIndex;
}

CellType MapData::GetCellType(std::size_t x, std::size_t y) const
{
	return tiles[GetTileIndex(x, y)].cellType;
}

bool MapData::GetLavaPossible(std::size_t x, std::size_t y) const
{
	return tiles[GetTileIndex(x, y)].bLavaPossible;
}


std::size_t MapData::GetTilesetIndex(std::size_t x, std::size_t y) const
{
	return tileInfos[GetTileInfoIndex(x, y)].tilesetIndex;
}

std::size_t MapData::GetImageIndex(std::size_t x, std::size_t y) const
{
	return tileInfos[GetTileInfoIndex(x, y)].tileIndex;
}

void MapData::TrimTilesetSources()
{
	tilesetSources.erase(
		std::remove_if(
			tilesetSources.begin(),
			tilesetSources.end(),
			[](TilesetSource tilesetSource) { return tilesetSource.IsEmpty(); }
		),
		tilesetSources.end()
	);
}


// Helper method to handle complex tile data indexing
std::size_t MapData::GetTileIndex(std::size_t x, std::size_t y) const
{
	auto lowerX = x & 0x1F; // ... 0001 1111
	auto upperX = x >> 5;   // ... 1110 0000
	return (upperX * mapTileHeight + y) * 32 + lowerX;
}
