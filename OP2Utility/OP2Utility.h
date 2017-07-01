#pragma once

#include "StringHelper.h"
#include "XFile.h"
#include "MapData.h"
#include "TileSetBmpReader.h"

#include "ArchiveHelper.h"
#include "Archives/CClmFile.h"
#include "Archives/CVolFile.h"

// OP2Utility is a utility library to aid in Outpost 2 functions typically not
// directly related to programming new scenarios. 

/*This includes
    Loading an Outpost 2 Map file into memory.
	Loading an Outpost 2 Tile Set BMP into memory.
	Cross platform file system support.
*/