#pragma once

// OP2 Utility Library Header

// OP2Utility is a cross platform C++ utility library to aid in Outpost 2 functions not
// directly related to programming new scenarios.

/*This includes
 - Outpost 2 archives (both CLM and VOL)
 - Outpost 2 map files
 - Outpost 2 tileset loading and conversion
 - Outpost 2 sprite loading (experimental)
 - File and memory stream manipulation
 - Cross platform file system support
 - Indexed Windows bitmap file support (expiremental)
*/

#include "../src/Archive/ArchiveFile.h"
#include "../src/Archive/ClmFile.h"
#include "../src/Archive/VolFile.h"

#include "../src/Map/Map.h"

#include "../src/Sprite/ArtFile.h"
#include "../src/Sprite/SpriteLoader.h"
#include "../src/Bitmap/BitmapFile.h"

#include "../src/Stream/FileReader.h"
#include "../src/Stream/SliceReader.h"
#include "../src/Stream/FileWriter.h"
#include "../src/Stream/MemoryReader.h"
#include "../src/Stream/MemoryWriter.h"

#include "../src/ResourceManager.h"
#include "../src/StringUtility.h"
#include "../src/XFile.h"
