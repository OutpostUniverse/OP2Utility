#pragma once

#include "MapData.h"
#include <string>

class SeekableStreamReader;

namespace MapReader {
	MapData Read(std::string filename, bool savedGame = false);
	MapData Read(SeekableStreamReader& mapStream, bool savedGame = false);
}
