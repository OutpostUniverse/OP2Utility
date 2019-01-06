#pragma once

#include "MapHeader.h"
#include "TileData.h"
#include "TilesetSource.h"
#include "TileInfo.h"
#include "TerrainType.h"
#include "../Rect.h"
#include "TileGroup.h"
#include <string>
#include <vector>
#include <cstddef>
#include <cstdint>

enum class CellType;

namespace Stream {
	class Writer;
	class Reader;
	class SeekableReader;
}

// FILE FORMAT DOCUMENTATION:
//     Outpost2SVN\OllyDbg\InternalData\FileFormat SavedGame and Map.txt.

// ALT IMPLEMENTATION (with COM support)
//     Outpost2SVN\MapEditor\OP2Editor.

//An Outpost 2 map file.
class MapData
{
public:
	MapData();
	MapData(const MapHeader& mapHeader);

	MapHeader CreateHeader() const;

	static void Write(const std::string& filename, const MapData& mapData);
	static void Write(Stream::Writer& mapStream, const MapData& mapData);
	static MapData ReadMap(std::string filename);
	static MapData ReadMap(Stream::Reader& seekableReader);
	static MapData ReadSavedGame(std::string filename);
	static MapData ReadSavedGame(Stream::SeekableReader& seekableReader);

	inline void SetVersionTag(int32_t versionTag) { MapData::versionTag = versionTag; };
	inline int32_t GetVersionTag() const { return versionTag; };
	inline bool IsSavedGame() const { return isSavedGame; };
	inline uint32_t MapTileWidth() const { return mapTileWidth; };
	inline uint32_t MapTileHeight() const { return mapTileHeight; };

	// Total number of tiles on map.
	inline std::size_t TileCount() const
	{
		return tiles.size();
	};

	// 1D listing of all tiles on the associated map. See MapHeader data for height and width of map.
	std::vector<TileData> tiles;

	// Represents playable area of the map.
	Rect clipRect;

	// Listing of all tile set sources associated with the map.
	std::vector<TilesetSource> tilesetSources;

	// Metadata about each available tile from the tile set sources.
	std::vector<TileInfo> tileInfos;

	// Listing of properties grouped by terrain type. Properties apply to a given range of tiles.
	std::vector<TerrainType> terrainTypes;

	std::vector<TileGroup> tileGroups;

public:
	std::size_t GetTileInfoIndex(std::size_t x, std::size_t y) const;
	CellType GetCellType(std::size_t x, std::size_t y) const;
	bool GetLavaPossible(std::size_t x, std::size_t y) const;
	std::size_t GetTilesetIndex(std::size_t x, std::size_t y) const;
	std::size_t GetImageIndex(std::size_t x, std::size_t y) const;

	void TrimTilesetSources();

private:
	int32_t versionTag;
	bool isSavedGame;
	uint32_t mapTileWidth;
	uint32_t mapTileHeight;

	std::size_t GetTileIndex(std::size_t x, std::size_t y) const;
	uint32_t GetWidthInTilesLog2(uint32_t widthInTiles) const;

	// Write
	static void WriteTilesetSources(Stream::Writer& streamWriter, const std::vector<TilesetSource>& tilesetSources);
	static void WriteTileGroups(Stream::Writer& streamWriter, const std::vector<TileGroup>& tileGroups);
	static void WriteContainerSize(Stream::Writer& streamWriter, std::size_t size);

	// Read
	static void SkipSaveGameHeader(Stream::SeekableReader& streamReader);
	static void ReadTilesetSources(Stream::Reader& streamReader, MapData& mapData, std::size_t tilesetCount);
	static void ReadTilesetHeader(Stream::Reader& streamReader);
	static void ReadVersionTag(Stream::Reader& streamReader);
	static void ReadTileGroups(Stream::Reader& streamReader, MapData& mapData);
	static TileGroup ReadTileGroup(Stream::Reader& streamReader);
};
