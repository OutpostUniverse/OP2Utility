#include "Map.h"
#include "MapHeader.h"
#include "CellType.h"
#include <algorithm>
#include <stdexcept>

namespace OP2Utility
{
Map::Map() :
	versionTag(MapHeader::MinMapVersion),
	isSavedGame(false),
	widthInTiles(0),
	heightInTiles(0) { }

std::size_t Map::GetTileMappingIndex(std::size_t x, std::size_t y) const
{
	return tiles[GetTileIndex(x, y)].tileMappingIndex;
}

CellType Map::GetCellType(std::size_t x, std::size_t y) const
{
	return tiles[GetTileIndex(x, y)].cellType;
}

void Map::SetCellType(CellType cellType, std::size_t x, std::size_t y)
{
	// Tube5 has the largest CellType index
	if (cellType > CellType::Tube5) {
		throw std::runtime_error("Improper cell type provided : CellType index = " + std::to_string(static_cast<int>(cellType)));
	}

	tiles[GetTileIndex(x, y)].cellType = cellType;
}

bool Map::GetLavaPossible(std::size_t x, std::size_t y) const
{
	return tiles[GetTileIndex(x, y)].bLavaPossible;
}

void Map::SetLavaPossible(bool lavaPossible, std::size_t x, std::size_t y)
{
	tiles[GetTileIndex(x, y)].bLavaPossible = lavaPossible;
}

std::size_t Map::GetTilesetIndex(std::size_t x, std::size_t y) const
{
	return tileMappings[GetTileMappingIndex(x, y)].tilesetIndex;
}

std::size_t Map::GetImageIndex(std::size_t x, std::size_t y) const
{
	return tileMappings[GetTileMappingIndex(x, y)].tileGraphicIndex;
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
	return (upperX * heightInTiles + y) * 32 + lowerX;
}

void Map::CheckMinVersionTag(uint32_t versionTag)
{
	if (versionTag < MapHeader::MinMapVersion)
	{
		throw std::runtime_error(
			"All instances of version tag in .map and .op2 files should be greater than " +
			std::to_string(MapHeader::MinMapVersion) + ".\n" +
			"Found version tag is " + std::to_string(versionTag) + "."
		);
	}
}
}
