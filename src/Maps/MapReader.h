#pragma once

#include "MapData.h"
#include <string>

namespace Stream {
	class SeekableStreamReader;
}

namespace MapReader {
	MapData Read(std::string filename, bool savedGame = false);
	MapData Read(Stream::SeekableStreamReader& mapStream, bool savedGame = false);
}
