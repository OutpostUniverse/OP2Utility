#pragma once

#include "MapData.h"
#include "MapType.h"
#include <string>

namespace Stream {
	class SeekableReader;
}

namespace MapReader {
	MapData Read(std::string filename, MapType type = MapType::Map);
	MapData Read(Stream::SeekableReader& mapStream, MapType type = MapType::Map);
}
