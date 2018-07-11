#pragma once

#include "Archives/ArchiveFile.h"
#include "Archives/ClmFile.h"
#include "Archives/VolFile.h"

#include "Maps/MapData.h"
#include "Maps/MapReader.h"
#include "Maps/MapWriter.h"

#include "Streams/FileStreamReader.h"
#include "Streams/FileSliceReader.h"
#include "Streams/FileStreamWriter.h"
#include "Streams/MemoryStreamReader.h"
#include "Streams/MemoryStreamWriter.h"

#include "ResourceManager.h"
#include "StringHelper.h"
#include "XFile.h"

// OP2Utility is a utility library to aid in Outpost 2 functions typically not
// directly related to programming new scenarios. 

/*This includes
    Loading an Outpost 2 Map file into memory.
	Handling Outpost 2 Archive files (clm and vol).
	Cross platform file system support.
*/
