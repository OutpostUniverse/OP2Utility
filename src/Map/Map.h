#pragma once

#include "Tile.h"
#include "TilesetSource.h"
#include "TileMapping.h"
#include "TerrainType.h"
#include "../Rect.h"
#include "TileGroup.h"
#include <string>
#include <vector>
#include <cstddef>
#include <cstdint>

enum class CellType;

struct MapHeader;

namespace Stream {
	class Writer;
	class Reader;
	class BidirectionalReader;
}

// FILE FORMAT DOCUMENTATION:
//     Outpost2SVN\OllyDbg\InternalData\FileFormat SavedGame and Map.txt.

// ALT IMPLEMENTATION (with COM support)
//     Outpost2SVN\MapEditor\OP2Editor.

//An Outpost 2 map file.
class Map
{
public:

	// 1D listing of all tiles on the associated map. See MapHeader data for height and width of map.
	std::vector<Tile> tiles;

	/**
	 * \brief	Represents the visible areas of the map.
	 *
	 * \note	Maps designated 'around the world' allow for continuous
	 *			scrolling on the X axis and so will populate X1 with -1
	 *			and X2 with \c INT_MAX.
	 */
	Rect clipRect;

	// Listing of all tile set sources associated with the map.
	std::vector<TilesetSource> tilesetSources;

	// Metadata about each available tile from the tile set sources.
	std::vector<TileMapping> tileMappings;

	// Listing of properties grouped by terrain type. Properties apply to a given range of tiles.
	std::vector<TerrainType> terrainTypes;

	std::vector<TileGroup> tileGroups;


	Map();

	static Map ReadMap(std::string filename);
	static Map ReadMap(Stream::Reader& mapStream);
	static Map ReadMap(Stream::Reader&& mapStream);

	static Map ReadSavedGame(std::string filename);
	static Map ReadSavedGame(Stream::BidirectionalReader& savedGameStream);
	static Map ReadSavedGame(Stream::BidirectionalReader&& savedGameStream);

	void Write(const std::string& filename) const;
	void Write(Stream::Writer& streamWriter) const;

	inline void SetVersionTag(uint32_t versionTag) { this->versionTag = versionTag; };
	inline uint32_t GetVersionTag() const { return versionTag; };
	inline bool IsSavedGame() const { return isSavedGame; };
	inline uint32_t WidthInTiles() const { return widthInTiles; };
	inline uint32_t HeightInTiles() const { return heightInTiles; };

	// Total number of tiles on map.
	inline std::size_t TileCount() const
	{
		return tiles.size();
	};

	std::size_t GetTileMappingIndex(std::size_t x, std::size_t y) const;
	CellType GetCellType(std::size_t x, std::size_t y) const;
	bool GetLavaPossible(std::size_t x, std::size_t y) const;
	std::size_t GetTilesetIndex(std::size_t x, std::size_t y) const;
	std::size_t GetImageIndex(std::size_t x, std::size_t y) const;

	static void CheckMinVersionTag(uint32_t versionTag);

	void TrimTilesetSources();

private:
	int32_t versionTag;
	bool isSavedGame;
	uint32_t widthInTiles;
	uint32_t heightInTiles;

	std::size_t GetTileIndex(std::size_t x, std::size_t y) const;

	// Write
	MapHeader CreateHeader() const;
	uint32_t GetWidthInTilesLog2(uint32_t widthInTiles) const;
	static void WriteTilesetSources(Stream::Writer& stream, const std::vector<TilesetSource>& tilesetSources);
	static void WriteTileGroups(Stream::Writer& stream, const std::vector<TileGroup>& tileGroups);
	static void WriteContainerSize(Stream::Writer& stream, std::size_t size);

	// Read
	static Map ReadMapBeginning(Stream::Reader& stream);
	static void SkipSaveGameHeader(Stream::BidirectionalReader& stream);
	static void ReadTilesetSources(Stream::Reader& stream, Map& map, std::size_t tilesetCount);
	static void ReadTilesetHeader(Stream::Reader& stream);
	static void ReadVersionTag(Stream::Reader& stream, uint32_t lastVersionTag);
	static void ReadSavedGameUnits(Stream::BidirectionalReader& stream);
	static void ReadTileGroups(Stream::Reader& stream, Map& map);
	static TileGroup ReadTileGroup(Stream::Reader& stream);
};
