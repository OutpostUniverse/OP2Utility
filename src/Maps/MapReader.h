#pragma once

#include "MapData.h"
#include <string>

namespace Stream {
	class SeekableReader;
}

namespace MapReader {
	MapData Read(std::string filename, bool savedGame = false);
	MapData Read(Stream::SeekableReader& mapStream, bool savedGame = false);
}
