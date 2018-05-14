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

	void ArchiveUnpacker::ExtractAllFiles(const char* destDirectory)
	{
		for (int i = 0; i < GetNumberOfPackedFiles(); ++i)
		{
			ExtractFile(i, XFile::ReplaceFilename(destDirectory, GetInternalFileName(i)).c_str());
		}
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

	void ArchiveUnpacker::CheckPackedFileIndexBounds(int fileIndex)
	{
		if (fileIndex < 0 || fileIndex >= m_NumberOfPackedFiles) {
			throw std::runtime_error("fileIndex is outside the bounds of packed files.");
		}
	}
}
