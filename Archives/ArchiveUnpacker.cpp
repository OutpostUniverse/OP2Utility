#include "ArchiveUnpacker.h"
#include "../XFile.h"

namespace Archives
{
	ArchiveUnpacker::ArchiveUnpacker(const char *fileName)
	{
		// Copy the filename to class storage
		m_VolumeFileName = new char[strlen(fileName) + 1];// Allocate space for the filename
		strcpy(m_VolumeFileName, fileName);				  // Copy the filename to class storage

		m_NumberOfPackedFiles = 0;
		m_VolumeFileSize = 0;
	}

	ArchiveUnpacker::~ArchiveUnpacker()
	{
		delete m_VolumeFileName;
	}

	int ArchiveUnpacker::ExtractAllFiles(const char* destDirectory)
	{
		for (int i = 0; i < GetNumberOfPackedFiles(); ++i)
		{
			if (ExtractFile(i, XFile::ReplaceFilename(destDirectory, GetInternalFileName(i)).c_str()) == false) {
				return false;
			}
		}

		return true;
	}

	bool ArchiveUnpacker::ContainsFile(const char* fileName)
	{
		for (int i = 0; i < GetNumberOfPackedFiles(); ++i)
		{
			if (XFile::PathsAreEqual(GetInternalFileName(i), fileName))
				return true;
		}

		return false;
	}
}
