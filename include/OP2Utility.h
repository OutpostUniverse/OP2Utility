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

#include "../src/Archive/ArchiveFile.h"
#include "../src/Archive/ClmFile.h"
#include "../src/Archive/VolFile.h"

#include "../src/Map/Map.h"

#include "../src/Sprite/ArtFile.h"
#include "../src/Sprite/OP2BmpLoader.h"
#include "../src/Bitmap/BitmapFile.h"

#include "../src/Stream/FileReader.h"
#include "../src/Stream/SliceReader.h"
#include "../src/Stream/FileWriter.h"
#include "../src/Stream/MemoryReader.h"
#include "../src/Stream/MemoryWriter.h"

#include "../src/ResourceManager.h"
#include "../src/StringHelper.h"
#include "../src/XFile.h"
