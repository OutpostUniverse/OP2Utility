#pragma once

#include "MapData.h"
#include <string>

namespace Stream {
	class StreamWriter;
}

// Writes an Outpost 2 map to file.
namespace MapWriter {
	void Write(const std::string& filename, const MapData& mapData);
	void Write(Stream::StreamWriter& mapStream, const MapData& mapData);
}
