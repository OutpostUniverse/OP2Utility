#include "MapHeader.h"

extern const int minMapVersion = 0x1010;

MapHeader::MapHeader() :
	versionTag(minMapVersion),
	bSavedGame(0),
	lgMapTileWidth(0),
	mapTileHeight(0),
	tilesetCount(0) {}
