#pragma once

#include "ArchivePacker.h"
#include "ArchiveUnpacker.h"

namespace Archives
{
	class ArchiveFile : public ArchiveUnpacker, public ArchivePacker
	{
	public:
		ArchiveFile(const std::string& filename) : ArchiveUnpacker(filename) {};
		virtual ~ArchiveFile() {};
	};
}
