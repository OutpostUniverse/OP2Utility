#pragma once

#include "MapData.h"
#include <string>

namespace Stream {
	class SeekableReader;
}

namespace MapReader {
	MapData ReadMap(std::string filename);
	MapData ReadMap(Stream::SeekableReader& seekableReader);
	MapData ReadSavedGame(std::string filename);
	MapData ReadSavedGame(Stream::SeekableReader& seekableReader);
}
