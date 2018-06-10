#pragma once

#include "MapData.h"
#include <string>
#include <vector>
#include <memory>

class SeekableStreamWriter;

// Writes an Outpost 2 map to file.
namespace MapWriter {
	void Write(const std::string& filename, const MapData& mapData);
	void Write(SeekableStreamWriter& mapStream, const MapData& mapData);
}
