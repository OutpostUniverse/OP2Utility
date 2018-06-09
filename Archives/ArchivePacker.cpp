#include "ArchivePacker.h"
#include "../XFile.h"
#include "../StringHelper.h"
#include <windows.h>
#include <stdexcept>

namespace Archives
{
	ArchivePacker::ArchivePacker() { }
	ArchivePacker::~ArchivePacker() { }

	// newFile has it's name changed to that of fileToReplace and the old file is deleted.
	// Returns true if successful and false otherwise
	bool ArchivePacker::ReplaceFileWithFile(const char *fileToReplace, const char *newFile)
	{
		std::string tempFileName("Temporary.vol");
		MoveFileA(fileToReplace, tempFileName.c_str());
		
		if (MoveFileA(newFile, fileToReplace) == 0) {
			return false;
		}

		DeleteFileA(tempFileName.c_str());

		return true;
	}

	std::vector<std::string> ArchivePacker::GetInternalNamesFromPaths(std::vector<std::string> paths)
	{
		std::vector<std::string> fileNames;

		for (const std::string& fileName : paths) {
			fileNames.push_back(XFile::GetFilename(fileName));
		}

		return fileNames;
	}

	void ArchivePacker::CheckSortedContainerForDuplicateNames(const std::vector<std::string>& internalNames) 
	{
		for (size_t i = 1; i < internalNames.size(); ++i)
		{
			if (StringHelper::CheckIfStringsAreEqual(internalNames[i - 1], internalNames[i])) {
				throw std::runtime_error("Unable to create an archive containing files with the same filename. Duplicate filename: " + internalNames[i]);
			}
		}
	}

	bool ArchivePacker::ComparePathFilenames(const std::string path1, const std::string path2)
	{
		return StringHelper::StringCompareCaseInsensitive(XFile::GetFilename(path1), XFile::GetFilename(path2));
	}
}
