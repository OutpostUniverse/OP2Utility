#pragma once

#include <string>

struct ArtFile;

namespace Stream {
	class SeekableWriter;
}

// Write the contents of an Outpost 2 .prt file
namespace ArtWriter {
	void Write(std::string filename, const ArtFile& artFile);
	void Write(Stream::SeekableWriter&, const ArtFile& artFile);
}
