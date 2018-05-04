#include "MapData.h"
#include "CellType.h"
#include "../XFile.h"
#include <stdexcept>

unsigned int MapData::GetTileInfoIndex(unsigned int x, unsigned int y) const
{
	size_t cellIndex = GetCellIndex(x, y);
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

size_t MapData::GetCellIndex(unsigned int x, unsigned int y) const
{
	unsigned int lowerX = x & 0x1F; // ... 0001 1111
	unsigned int upperX = x >> 5;   // ... 1110 0000
	return (upperX * header.mapTileHeight + y) * 32 + lowerX;
}

void MapData::AddTileset(std::string filename, int tileCount)
{
	AddTilesetSource(filename, tileCount);

	for (int i = 0; i < tileCount; ++i)
	{
		tileInfos.push_back(TileInfo());
	}
}

void MapData::AddTilesetSource(std::string filename, int tileCount)
{
	for (TilesetSource& tilesetSource : tilesetSources)
	{
		if (tilesetSource.IsSet()) {
			continue;
		}

		tilesetSource.Set(filename, tileCount);
		return;
	}

	throw std::runtime_error("Map already contains 512 tilesets.");
}

void MapData::RemoveTileset(std::string filename)
{
	rsize_t tilesetIndex = GetTilesetIndex(filename);
	
	if (tilesetIndex == -1) {
		return; //tilesetSource was not found.
	}

	size_t firstTileInfoIndexRemoved = 0;
	for (size_t i = 0; i < tilesetIndex; ++i) {
		firstTileInfoIndexRemoved += tilesetSources[i].tileCount;
	}

	TileRange range(firstTileInfoIndexRemoved, firstTileInfoIndexRemoved + tilesetSources[tilesetIndex].tileCount);

	RemoveTilesetSource(tilesetIndex);
	RemoveTileInfos(tilesetIndex);
	UpdateTilesAfterTilesetRemoval(range);
	UpdateTileGroupsAfterTilesetRemoval(range);
	UpdateTerrainTypesAfterTilesetRemoval(range, filename);
}

// returns the index of the tilesetSource. 
// If the tilesetSource is not located, -1 is returned.
size_t MapData::GetTilesetIndex(std::string filename)
{
	filename = XFile::ChangeFileExtension(filename, "");

	for (size_t i = 0; i < tilesetSources.size(); ++i)
	{
		// return if all currently set tilesetSources are reviewed.
		if (!tilesetSources[i].IsSet()) {
			return -1;
		}

		if (XFile::PathsAreEqual(filename, tilesetSources[i].tilesetFilename)) {
			return i;
		}
	}

	// Return -1 if all tilesetSources are currently set but none contain the given filename.
	return -1;
}

void MapData::RemoveTilesetSource(size_t tilesetIndex)
{
	tilesetSources.erase(tilesetSources.begin() + tilesetIndex);
	tilesetSources.push_back(TilesetSource()); // Add an empty tileset to the end of the vector.
}

void MapData::RemoveTileInfos(size_t tilesetIndex)
{
	for (int i = tileInfos.size() - 1; i >= 0; --i)
	{
		if (tileInfos[i].tilesetIndex == tilesetIndex)
		{
			tileInfos.erase(tileInfos.begin() + i);
		}
		else if (tileInfos[i].tilesetIndex > tilesetIndex)
		{
			tileInfos[i].tilesetIndex--;
		}
	}
}

void MapData::UpdateTilesAfterTilesetRemoval(const TileRange& tileRange)
{
	for (TileData& tileData : tiles)
	{
		// Tile was removed completely
		if (tileRange.Includes(tileData.tileIndex))
		{
			tileData.Reset();
		}
		// Tile's index must be reduced by range of tileset removed.
		else if (tileData.tileIndex > tileRange.end)
		{
			tileData.tileIndex -= tileRange.Range();
		}
	}
}

void MapData::UpdateTileGroupsAfterTilesetRemoval(const TileRange& tileRange)
{
	for (TileGroup& tileGroup : tileGroups)
	{
		for (size_t mappingIndex : tileGroup.mappingIndices)
		{
			// Tile was removed completely
			if (tileRange.Includes(mappingIndex))
			{
				mappingIndex = 0;
			}
			// Tile's index must be reduced by range of tileset removed.
			else if (mappingIndex > tileRange.end)
			{
				mappingIndex -= tileRange.Range();
			}
		}
	}
}

void MapData::UpdateTerrainTypesAfterTilesetRemoval(const TileRange& rangeRemoved, std::string tilesetName)
{
	for (TerrainType& terrainType : terrainTypes)
	{
		if (terrainType.tileRange.Includes(rangeRemoved))
		{
			throw std::runtime_error("Unable to remove the tileset " + tilesetName + " because it belongs to a TerrainType.");
		}

		if (terrainType.tileRange.start > rangeRemoved.start)
		{
			terrainType.bulldozedTileIndex -= rangeRemoved.Range();
			terrainType.lavaTileIndex -= rangeRemoved.Range();
			terrainType.rubbleTileIndex -= rangeRemoved.Range();
			terrainType.scorchedTileIndex -= rangeRemoved.Range();

			for (int i = 0; i < 16; ++i)
			{
				terrainType.tube[i] -= rangeRemoved.Range();
			}

			for (int i = 0; i < 6; ++i)
			{
				terrainType.tubeTiles[i] -= rangeRemoved.Range();
			}

			for (int i = 0; i < 5; ++i)
			{
				for (int j = 0; j < 16; ++j)
				{
					terrainType.wall[i][j] -= rangeRemoved.Range();
				}
			}

		}
	}
}
