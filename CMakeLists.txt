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
	"src/XFile.cpp"
	"src/XFile.h"
	
	"src/Archives/AdaptHuffTree.cpp"
	"src/Archives/AdaptHuffTree.h"
	"src/Archives/ArchiveFile.h"
	"src/Archives/ArchivePacker.cpp"
	"src/Archives/ArchivePacker.h"
	"src/Archives/ArchiveUnpacker.cpp"
	"src/Archives/BitStream.cpp"
	"src/Archives/BitStream.h"
	"src/Archives/ClmFile.cpp"
	"src/Archives/ClmFile.h"
	"src/Archives/CompressionType.h"
	"src/Archives/HuffLZ.cpp"
	"src/Archives/HuffLZ.h"
	"src/Archives/VolFile.cpp"
	"src/Archives/VolFile.h"
	"src/Archives/WaveFile.h"
	
	"src/Maps/CellType.h"
	"src/Maps/Rect.h"
	"src/Maps/MapData.cpp"
	"src/Maps/MapData.h"
	"src/Maps/MapHeader.h"
	"src/Maps/MapReader.cpp"
	"src/Maps/MapReader.h"
	"src/Maps/MapWriter.cpp"
	"src/Maps/MapWriter.h"
	"src/Maps/TerrainType.h"
	"src/Maps/TileData.h"
	"src/Maps/TileGroup.h"
	"src/Maps/TileInfo.h"
	"src/Maps/TilesetSource.h"
	
	"src/Streams/FileSliceReader.cpp"
	"src/Streams/FileSliceReader.h"
	"src/Streams/FileStreamReader.cpp"
	"src/Streams/FileStreamReader.h"
	"src/Streams/FileStreamWriter.cpp"
	"src/Streams/FileStreamWriter.h"
	"src/Streams/MemoryStreamReader.cpp"
	"src/Streams/MemoryStreamReader.h"
	"src/Streams/MemoryStreamWriter.cpp"
	"src/Streams/MemoryStreamWriter.h"
	"src/Streams/SeekableStreamReader.h"
	"src/Streams/SeekableStreamWriter.h"
	"src/Streams/StreamReader.h"
	"src/Streams/StreamWriter.h")
	
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
