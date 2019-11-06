# OP2Utility

A cross platform C++17 utility library that simplifies Outpost 2 game resource manipulation, including archives (vol and clm), maps, graphics, sounds, and music. This is a static library designed for use by other code projects.

See [OP2Archive](https://github.com/OutpostUniverse/OP2Archive) and [OP2MapImager](https://github.com/OutpostUniverse/OP2MapImager) for examples of standalone utilities using OP2Utility.

OP2Utility does not relate to mission building or extension modules for Outpost 2. For these applications see the [OP2 Mission SDK](https://github.com/OutpostUniverse/OP2MissionSDK) and [op2ext](https://github.com/OutpostUniverse/op2ext) repositories respectively. However, this library could be used in conjunction to aid resource management of custom missions or game extension modules.

Main features:
 - Extract and repack files in .VOL archives
 - Extract and repack music in .CLM archives
 - Decompress data from .VOL archives (no current support for re-compression)
 - Load and parse map files to in-memory format, and save back to disk
 - Load and parse the map portion of saved game files to in-memory format
 - Load of tileset graphics (both standard Windows bitmaps and custom OP2 format)
 - Load of OP2 specific sprite formats (.prt/.bmp) (partial experimental support)
 - General file stream reading/writing
 - Indexed Bitmap manipulation (experimental)

The library can be compiled and used on both Windows and Linux. Both x86 (32-bit) and x64 architectures are fully supported. Partial unit test coverage is made using [Google Test](https://github.com/google/googletest). Supported compilers:
 - Visual Studio 2017
 - GCC
 - Clang

All headers meant for consumption outside of OP2Utility are contained in **include/OP2Utility.h**. No other library headers should be needed by 3rd party applications for normal use.

This library is released under a MIT license. See License.txt for details.

## Sample Code
The `auto` keyword is avoided in example code to visually highlight what type is returned. This is not intended as a style recommendation, but to ease learning.

### Archive File Overview
Volume and CLM files are represented by the classes VolFile and ClmFile. These both inherit from ArchiveFile, which represents a generic archive that can be extracted from.

#### Major public members:
 * `ArchiveFile` - Base class for VolFile and ClmFile. Can be used to complete basic extract and list commands that pertain to both types of archives.
 * `VolFile` - Represents a Volume File.
 * `ClmFile` - Represents a CLM File.
 * `CompressionType` - Enum dictating the compression algorithm used by a Volume.

#### Volume Archive Manipulation

Volumes may either be compressed or uncompressed. Outpost 2 contains references to 3 types of compression, RLE (Run - Length Encoded), LZ (Lempel - Ziv), and LZH (Lempel - Ziv, with adaptive Huffman encoding). Only LZH is used in practice. Current releases of Outpost 2 include all volumes repackaged in uncompressed format to ease modding.

OP2Utility cannot create compressed archives.

#### Volume Archive Example Code
```C++
#include "OP2Utility.h"

// Open a Volume file into memory
VolFile volFile("maps.vol");

// Extract the file contained at index 0 of the volume
volFile.ExtractFile(0, "/Test/);

// Read a map into memory without extracting the archived file to disk
Map::ReadMap(volFile.OpenStream(0));
```

#### CLM File Manipulation

Outpost 2 stores all music tracks except for the main menu track in the archive file op2.clm. As a starting point for manipulating audio tracks for Outpost 2, consider the free program Audacity (http://www.audacityteam.org/). 

When naming audio files for storage in a CLM archive, the filename (excluding extension) must be 8 characters or less. The names of the audio tracks provided with the stock download of the game must be used. The game will then select the music tracks as it sees appropriate based on name.

Outpost 2 audio tracks must be formatted as WAV files with the following settings: 
 * Frequency = 22050 Hz
 * Channels = 1 (Mono)
 * BitsPerSample = 16

The file size of each WAV audio track (the actual wav data) must be zero padded to a multiple of 32768 bytes (0x8000). Output 2 fills the music buffer in 0x8000 size chunks and will not zero fill if a chunk at the end of a file is too short. If the file is not a multiple of 32768 bytes (0x8000), some garbage data will be played in Outpost 2 before switches to the next track. *OP2Utility currently does not auto-handle padding of provided WAV tracks.*

#### CLM File Manipulation

Exact same as Volume File manipulation

### Resource Manager
The Resource Manager eases retrieving Outpost 2 files when the consumer does not know if the file is stored loosely on disk or in an archive. 

Each `ResourceManager` is passed a base directory where searches are made from. After construction, relative directories passed into `ResourceManager` will search relative to the established base directory, *not* the program's overall environmentally set directory.

#### Resource Manager Example Code
```C++
#include "OP2Utility.h"

// Creates a ResourceManager whose base directory is EnvironmentDirectory/TestDirectory
ResourceManager resourceManager("TestDirectory");

// Load a map into memory without extracting the file to disk if it is contained in an archive.
Map map(resourceManager.GetResourceStream("MesaMissions.map"));

// Same as above, but only look for an already extracted file
map(resourceManager.GetResourceStream("MesaMissions.map", false));

// Retrieve all filenames (loose or in an archive) that contain the word Eden via C++ std library Regex search.
std::vector<std::string> filenames = GetAllFilenames("Eden");

// Retrieve all files containing a .map extension
std::vector<std::string> filenames GetAllFilenamesOfType(".map");

// Return the filename of the archive containing the passed filename. Returns an empty string if file is not located in an archive file in the ResourceManager's working directory.
auto archiveFilename = FindContainingArchivePath(const std::string& filename);
if (!archiveFilename.empty()) {
    // TODO
}

// Return a list of the filenames of all archives loaded by the ResourceManager
std::vector<std::string> archiveFilenames = GetArchiveFilenames();
```

### Map and Saved Game File Manipulation
OP2Utility allows loading Outpost 2 map (.map) files into memory. After loaded into memory, different map properties such as width, tilesets, and cell indexed can be read or manipulated. 

Outpost 2 saved games (.op2) have a similar format to map files. OP2Utility supports partial reading and manipulation of saved game files, mostly where the map and saved game format overlap. 

```C++
#include "OP2Utility.h"

// Load a map into memory
Map op2Map = Map::ReadMap("MesaMissions.map");

// Write modified map to file
op2Map.Write("MesaMissionsRevB.map);

// Load parts of saved game that are equivalent to the map format into memory.
Map savedGame = Map::ReadSavedGame(std::string filename);

// Change the map version tag.
op2Map.SetVersionTag(100);

// Retrieve the version tag 
uint32_t versionTag = op2Map.GetVersionTag();

// Check if loaded file is a saved game or map
if (op2Map.IsSavedGame()) { 
    // TODO
};

// Retrieve map width and height in tiles
uint32_t widthInTiles = op2Map.WidthInTiles();
uint32_t heightInTiles = op2Map.HeightInTiles();
std::size_t tileCount = op2Map.TileCount();

std::size_t GetTileMappingIndex(std::size_t x, std::size_t y) const;
CellType GetCellType(std::size_t x, std::size_t y) const;
bool GetLavaPossible(std::size_t x, std::size_t y) const;
std::size_t GetTilesetIndex(std::size_t x, std::size_t y) const;
std::size_t GetImageIndex(std::size_t x, std::size_t y) const;
```

### Outpost 2 specific Art/Bitmap File Manipulation


### Stream Read/Write Operations
