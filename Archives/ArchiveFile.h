#pragma once

#include "ArchivePacker.h"
#include "ArchiveUnpacker.h"

namespace Archives
{
	class ArchiveFile : public ArchiveUnpacker, public ArchivePacker
	{
	public:
		ArchiveFile(const std::string& fileName) : ArchiveUnpacker(fileName) {};
		virtual ~ArchiveFile() {};
	};
}
