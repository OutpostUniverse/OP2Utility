#include "MapHeader.h"

#include <cmath>

unsigned int MapHeader::mapTileWidth()
{
	return (unsigned int)pow(2, lgMapTileWidth);
}