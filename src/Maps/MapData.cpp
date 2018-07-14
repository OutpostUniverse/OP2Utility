#include "MapData.h"
#include "CellType.h"

unsigned int MapData::GetTileInfoIndex(unsigned int x, unsigned int y) const
{
	std::size_t cellIndex = GetCellIndex(x, y);
	return tiles[cellIndex].tileIndex;
}

CellType MapData::GetCellType(unsigned int x, unsigned int y) const
{
	return tiles[GetCellIndex(x, y)].cellType;
}

int MapData::GetLavaPossible(unsigned int x, unsigned int y) const
{
	return tiles[GetCellIndex(x, y)].bLavaPossible;
}

short MapData::GetTilesetIndex(unsigned int x, unsigned int y) const
{
	unsigned int tileInfoIndex = GetTileInfoIndex(x, y);
	return tileInfos[tileInfoIndex].tilesetIndex;
}

short MapData::GetImageIndex(unsigned int x, unsigned int y) const
{
	unsigned int tileInfoIndex = GetTileInfoIndex(x, y);
	return tileInfos[tileInfoIndex].tileIndex;
}

std::size_t MapData::GetCellIndex(unsigned int x, unsigned int y) const
{
	unsigned int lowerX = x & 0x1F; // ... 0001 1111
	unsigned int upperX = x >> 5;   // ... 1110 0000
	return (upperX * header.mapTileHeight + y) * 32 + lowerX;
}
