#pragma once

#include "MapData.h"
#include <string>
#include <vector>
#include <memory>

class SeekableStreamReader;

class MapReader
{
public:
	MapData Read(std::unique_ptr<SeekableStreamReader> mapStream, bool savedGame = false);
	MapData Read(std::string filename, bool savedGame = false);

private:
	std::unique_ptr<SeekableStreamReader> streamReader;

	void SkipSaveGameHeader();
	void ReadHeader(MapData& mapData);
	void ReadTiles(MapData& mapData);
	void ReadClipRect(const ClipRect& clipRect);
	void ReadTilesetSources(MapData& mapData);
	void ReadTilesetHeader();
	void ReadTileInfo(MapData& mapData);
	void ReadVersionTag();
	void ReadTileGroups(MapData& mapData);
	TileGroup ReadTileGroup();
	std::string ReadString();
};
