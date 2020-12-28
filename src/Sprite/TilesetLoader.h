#pragma once

#include "../Tag.h"

namespace Stream {
	class BidirectionalReader;
}


static class TilesetLoader
{
public:
	static constexpr Tag TagFileSignature = MakeTag("PBMP");

	static bool PeekIsCustomTileset(Stream::BidirectionalReader& reader);
};
