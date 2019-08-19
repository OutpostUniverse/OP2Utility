# OP2Utility

A C++17 utility library for working with Outpost 2 game resources (maps, graphics, sounds). This is a static library to be used by other code projects.

See [OP2Archive](https://github.com/OutpostUniverse/OP2Archive) and [OP2MapImager](https://github.com/OutpostUniverse/OP2MapImager) for examples of standalone utilities that make use of this library.

This does not relate to mission building or extension modules for Outpost 2. For that see the [OP2 Mission SDK](https://github.com/OutpostUniverse/OP2MissionSDK) and [op2ext](https://github.com/OutpostUniverse/op2ext) respectively. Though with that said, if you wanted to be creative with game resources, you could potentially use this library from a mission or an extension module.

Main features:
 - Extract and repack files in .VOL archives
 - Extract and repack music in .CLM archives
 - Decompress data from .VOL archives (no current support for re-compression)
 - Load and parse map files to in-memory format, and save back to disk
 - Load and parse the map portion of saved game files to in-memory format
 - Load of tileset graphics (both standard Windows bitmaps and custom OP2 format)
 - Load of spite graphics (partial experimental support)
 - General file stream reading/writing

The library can be compiled and used on both Windows and Linux. Both x86 (32-bit) and x64 architectures are fully supported. Partial unit test coverage is made using [Google Test](https://github.com/google/googletest). Supported compilers:
 - Visual Studio 2017
 - GCC
 - Clang

All headers meant for consumption outside of OP2Utility are contained in **include/OP2Utility.h**. No other library headers should be needed by 3rd party applications for normal use.

This library is released under a MIT license. See License.txt for details.

## Change Log

Version 1.0.1

* Added MIT License to project. See License.txt for details.

Version 1.0.0

* Initial Release
