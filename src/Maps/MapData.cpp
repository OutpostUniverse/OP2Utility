#include "MapData.h"
#include "CellType.h"

unsigned int MapData::GetTileInfoIndex(std::size_t x, std::size_t y) const
{
	std::size_t cellIndex = GetCellIndex(x, y);
	return tiles[cellIndex].tileIndex;
}

CellType MapData::GetCellType(std::size_t x, std::size_t y) const
{
	return tiles[GetCellIndex(x, y)].cellType;
}

int MapData::GetLavaPossible(std::size_t x, std::size_t y) const
{
	return tiles[GetCellIndex(x, y)].bLavaPossible;
}

uint16_t MapData::GetTilesetIndex(std::size_t x, std::size_t y) const
{
	auto tileInfoIndex = GetTileInfoIndex(x, y);
	return tileInfos[tileInfoIndex].tilesetIndex;
}

uint16_t MapData::GetImageIndex(std::size_t x, std::size_t y) const
{
	auto tileInfoIndex = GetTileInfoIndex(x, y);
	return tileInfos[tileInfoIndex].tileIndex;
}

std::size_t MapData::GetCellIndex(std::size_t x, std::size_t y) const
{
	std::size_t lowerX = x & 0x1F; // ... 0001 1111
	std::size_t upperX = x >> 5;   // ... 1110 0000
	return (upperX * header.mapTileHeight + y) * 32 + lowerX;
}
