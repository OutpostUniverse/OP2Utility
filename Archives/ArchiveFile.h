#pragma once

#include "ArchivePacker.h"
#include "ArchiveUnpacker.h"

namespace Archives
{
	class ArchiveFile : public ArchiveUnpacker, public ArchivePacker
	{
	public:
		ArchiveFile(const char *fileName) : ArchiveUnpacker(fileName) {};
		virtual ~ArchiveFile() {};
	};
}
