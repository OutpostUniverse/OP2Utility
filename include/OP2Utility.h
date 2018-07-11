#pragma once

// OP2 Utility Library Header

// OP2Utility is a cross platform C++ utility library to aid in Outpost 2 functions not
// directly related to programming new scenarios. 

/*This includes
 - Handling Outpost 2 Archvies (both CLM and VOL)
 - Handling Outpost 2 map files
 - File and Memory Stream manipulation
 - Cross platform file system support
*/

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
