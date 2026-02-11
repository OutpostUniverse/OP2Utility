#include "Reader.h"


namespace OP2Utility::Stream
{
	std::string Reader::ReadNullTerminatedString(std::size_t maxCount)
	{
		std::string str;

		char c;
		for (std::size_t i = 0; i < maxCount; ++i)
		{
			Read(c);
			if (c == '\0') {
				break;
			}

			str.push_back(c);
		}

		return str;
	}
}
