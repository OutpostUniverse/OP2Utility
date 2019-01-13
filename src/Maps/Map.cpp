#include "Map.h"
#include "MapHeader.h"
#include "CellType.h"
#include <algorithm>

Map::Map() :
	versionTag(MapHeader::MinMapVersion),
	isSavedGame(false),
	mapTileWidth(0),
	mapTileHeight(0) { }

std::size_t Map::GetTileInfoIndex(std::size_t x, std::size_t y) const
{
	return tiles[GetTileIndex(x, y)].tileIndex;
}

CellType Map::GetCellType(std::size_t x, std::size_t y) const
{
	return tiles[GetTileIndex(x, y)].cellType;
}

bool Map::GetLavaPossible(std::size_t x, std::size_t y) const
{
	return tiles[GetTileIndex(x, y)].bLavaPossible;
}


std::size_t Map::GetTilesetIndex(std::size_t x, std::size_t y) const
{
	return tileInfos[GetTileInfoIndex(x, y)].tilesetIndex;
}

std::size_t Map::GetImageIndex(std::size_t x, std::size_t y) const
{
	return tileInfos[GetTileInfoIndex(x, y)].tileIndex;
}

void Map::TrimTilesetSources()
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
std::size_t Map::GetTileIndex(std::size_t x, std::size_t y) const
{
	auto lowerX = x & 0x1F; // ... 0001 1111
	auto upperX = x >> 5;   // ... 1110 0000
	return (upperX * mapTileHeight + y) * 32 + lowerX;
}
