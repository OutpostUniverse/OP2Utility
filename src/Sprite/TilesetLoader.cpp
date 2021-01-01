#include "TilesetLoader.h"
#include "../Bitmap/BitmapFile.h"
#include "../Stream/BidirectionalReader.h"
#include <stdexcept>

namespace Tileset
{
	bool PeekIsCustomTileset(Stream::BidirectionalReader& reader)
	{
		Tag tag;
		reader.Read(tag);
		reader.SeekBackward(sizeof(tag));

		return tag == TagFileSignature;
	}

	void ValidateTileset(const BitmapFile& tileset)
	{
		if (tileset.imageHeader.bitCount != 8)
		{
			throw std::runtime_error("Tileset palette must be an 8 bit indexed bitmap");
		}

		if (tileset.imageHeader.width != 32)
		{
			throw std::runtime_error("Tileset width must be exactly 32 pixels");
		}

		if (tileset.imageHeader.height % 32 != 0)
		{
			throw std::runtime_error("Tileset height must be a multiple of 32 pixels");
		}
	}
}
