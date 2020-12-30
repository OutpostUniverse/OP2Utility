#pragma once

#include "../Tag.h"

namespace Stream {
	class BidirectionalReader;
}


namespace TilesetLoader
{
	constexpr Tag TagFileSignature = MakeTag("PBMP");

	bool PeekIsCustomTileset(Stream::BidirectionalReader& reader);
}
