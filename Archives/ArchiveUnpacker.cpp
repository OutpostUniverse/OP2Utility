#include "ArchiveUnpacker.h"
#include "../XFile.h"

namespace Archives
{
	ArchiveUnpacker::ArchiveUnpacker(const std::string& fileName)
	{
		this->m_ArchiveFileName = fileName;

		m_NumberOfPackedFiles = 0;
		m_ArchiveFileSize = 0;
	}

	ArchiveUnpacker::~ArchiveUnpacker() { }

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
