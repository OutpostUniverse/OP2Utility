#pragma once

#include "ArtFile.h"
#include <string>

namespace Stream {
	class SeekableReader;
}

// Read the contents of an Outpost 2 .prt file
namespace ArtReader {
	ArtFile Read(std::string filename);
	ArtFile Read(Stream::SeekableReader& seekableReader);
};
