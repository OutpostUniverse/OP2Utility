#include "TilesetLoader.h"
#include "../Stream/BidirectionalReader.h"


namespace TilesetLoader
{
	bool PeekIsCustomTileset(Stream::BidirectionalReader& reader)
	{
		Tag tag;
		reader.Read(tag);
		reader.SeekBackward(sizeof(tag));

		return tag == TagFileSignature;
	}
}
