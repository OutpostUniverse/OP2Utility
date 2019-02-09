cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project("OP2Utility")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# https://cmake.org/cmake/help/v3.12/command/file.html
# CMake documentation recommends not collecting all source files using a GLOB
file(GLOB SOURCE_CODE
	"src/ResourceManager.cpp"
	"src/ResourceManager.h"
	"src/StringHelper.cpp"
	"src/StringHelper.h"
	"src/Tag.h"
	"src/XFile.cpp"
	"src/XFile.h"

	"src/Archive/AdaptiveHuffmanTree.cpp"
	"src/Archive/AdaptiveHuffmanTree.h"
	"src/Archive/ArchiveFile.h"
	"src/Archive/ArchivePacker.cpp"
	"src/Archive/ArchivePacker.h"
	"src/Archive/ArchiveUnpacker.cpp"
	"src/Archive/BitStreamReader.cpp"
	"src/Archive/BitStreamReader.h"
	"src/Archive/ClmFile.cpp"
	"src/Archive/ClmFile.h"
	"src/Archive/CompressionType.h"
	"src/Archive/HuffLZ.cpp"
	"src/Archive/HuffLZ.h"
	"src/Archive/VolFile.cpp"
	"src/Archive/VolFile.h"
	"src/Archive/WaveFile.h"

	"src/Map/CellType.h"
	"src/Map/Rect.h"
	"src/Map/Map.cpp"
	"src/Map/Map.h"
	"src/Map/MapHeader.h"
	"src/Map/MapReader.cpp"
	"src/Map/MapReader.h"
	"src/Map/MapWriter.cpp"
	"src/Map/MapWriter.h"
	"src/Map/TerrainType.h"
	"src/Map/TileData.h"
	"src/Map/TileGroup.h"
	"src/Map/TileInfo.h"
	"src/Map/TilesetSource.h"

	"src/Stream/FileSliceReader.h"
	"src/Stream/FileStreamReader.cpp"
	"src/Stream/FileStreamReader.h"
	"src/Stream/FileStreamWriter.cpp"
	"src/Stream/FileStreamWriter.h"
	"src/Stream/MemoryStreamReader.cpp"
	"src/Stream/MemoryStreamReader.h"
	"src/Stream/MemoryStreamWriter.cpp"
	"src/Stream/MemoryStreamWriter.h"
	"src/Stream/SeekableStreamReader.h"
	"src/Stream/SeekableStreamWriter.h"
	"src/Stream/StreamReader.h"
	"src/Stream/StreamWriter.h")

file(GLOB EXTERNAL_HEADERS "include/OP2Utility.h")

add_library(OP2Utility STATIC "${SOURCE_CODE}" "${EXTERNAL_HEADERS}")

target_include_directories(OP2Utility PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

IF (NOT MSVC) #stdc++fs creates a LNK1104 on Visual Studio 2017
target_link_libraries(${PROJECT_NAME} stdc++fs)
ENDIF()

# Set Visual Studio to using static code generation in all default Cmake generated configurations
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

# Add filters (virtual folders) in Visual Studio that correspond in name to actual directories
# https://cmake.org/cmake/help/v3.12/command/source_group.html
set(SRC_ROOT_PATH "${OP2Utility_SOURCE_DIR}/src")

foreach(source IN ITEMS ${SOURCE_CODE})
    get_filename_component(source_path "${source}" PATH)
    file(RELATIVE_PATH source_path_rel "${SRC_ROOT_PATH}" "${source_path}")
    string(REPLACE "/" "\\" group_path "${source_path_rel}")
    source_group("${group_path}" FILES "${source}")
endforeach()

# Create a separate Visual Studio filter for the External Header(s) not contained within the src directory
source_group("include" FILES "${EXTERNAL_HEADERS}")
