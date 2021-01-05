#include "TilesetCommon.h"
#include <stdexcept>

namespace Tileset
{
	void throwReadError(std::string propertyName, std::string value, std::string expectedValue)
	{
		throw std::runtime_error(formatReadErrorMessage(propertyName, value, expectedValue));
	}

	void throwReadError(std::string propertyName, uint32_t value, uint32_t expectedValue)
	{
		throwReadError(propertyName, std::to_string(value), std::to_string(expectedValue));
	}

	std::string formatReadErrorMessage(std::string propertyName, std::string value, std::string expectedValue)
	{
		return "Tileset property " + propertyName + " reads. Expected a value of " + expectedValue + ".";
	}
}
