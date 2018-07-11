#include "ArchiveUnpacker.h"
#include "../XFile.h"
#include <cstddef>

namespace Archives
{
	ArchiveUnpacker::ArchiveUnpacker(const std::string& fileName) :
		m_ArchiveFileName(fileName), m_NumberOfPackedFiles(0), m_ArchiveFileSize(0) { }

	ArchiveUnpacker::~ArchiveUnpacker() { }

	void ArchiveUnpacker::ExtractAllFiles(const std::string& destDirectory)
	{
		for (int i = 0; i < GetNumberOfPackedFiles(); ++i)
		{
			ExtractFile(i, XFile::ReplaceFilename(destDirectory, GetInternalFileName(i)));
		}
	}

	bool ArchiveUnpacker::ContainsFile(const std::string& fileName)
	{
		for (int i = 0; i < GetNumberOfPackedFiles(); ++i)
		{
			if (XFile::PathsAreEqual(GetInternalFileName(i), fileName)) {
				return true;
			}
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
