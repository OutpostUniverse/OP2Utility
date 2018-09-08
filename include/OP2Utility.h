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

#include "../src/Archives/ArchiveFile.h"
#include "../src/Archives/ClmFile.h"
#include "../src/Archives/VolFile.h"

#include "../src/Maps/MapData.h"
#include "../src/Maps/MapReader.h"
#include "../src/Maps/MapWriter.h"

#include "../src/Sprites/ArtFile.h"

#include "../src/Streams/FileReader.h"
#include "../src/Streams/FileSliceReader.h"
#include "../src/Streams/FileWriter.h"
#include "../src/Streams/MemoryReader.h"
#include "../src/Streams/MemoryWriter.h"

#include "../src/ResourceManager.h"
#include "../src/StringHelper.h"
#include "../src/XFile.h"
