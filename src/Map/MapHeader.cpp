#include "MapHeader.h"

MapHeader::MapHeader() :
	versionTag(CurrentMapVersion),
	bSavedGame(0),
	lgWidthInTiles(0),
	heightInTiles(0),
	tilesetCount(0) {}
