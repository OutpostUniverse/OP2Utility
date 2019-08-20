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
 - Load of spite graphics (partial experimental support)
 - General file stream reading/writing

The library can be compiled and used on both Windows and Linux. Both x86 (32-bit) and x64 architectures are fully supported. Partial unit test coverage is made using [Google Test](https://github.com/google/googletest). Supported compilers:
 - Visual Studio 2017
 - GCC
 - Clang

All headers meant for consumption outside of OP2Utility are contained in **include/OP2Utility.h**. No other library headers should be needed by 3rd party applications for normal use.

This library is released under a MIT license. See License.txt for details.
