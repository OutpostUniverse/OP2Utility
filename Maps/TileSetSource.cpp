#include "TileSetSource.h"
#include "../XFile.h"
#include <stdexcept>

void TilesetSource::Set(std::string filename, int tileCount)
{
	if (tileCount < 0) {
		std::runtime_error("A tileset's tileCount cannot be negative");
	}

	// filenames stored in TileSet do not include the file's extension.
	filename = XFile::ChangeFileExtension(filename, "");

	if (filename.size() < 1 || filename.size() > 8) {
		std::runtime_error("A tileset's filename cannot be empty and cannot be greater than 8 characters, not including the file extension");
	}

	tilesetFilename = filename;
	this->tileCount = tileCount;
}

void TilesetSource::Remove()
{
	tilesetFilename.clear();
	tileCount = 0;
}