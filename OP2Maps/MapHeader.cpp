#include "MapHeader.h"

#include <cmath>

unsigned int MapHeader::MapTileWidth()
{
	return (unsigned int)pow(2, lgMapTileWidth);
}