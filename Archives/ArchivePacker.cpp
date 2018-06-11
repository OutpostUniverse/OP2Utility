#include "ArchivePacker.h"
#include "../XFile.h"
#include "../StringHelper.h"
#include <cstdio>
#include <stdexcept>

namespace Archives
{
	ArchivePacker::ArchivePacker() { }
	ArchivePacker::~ArchivePacker() { }

	// newFile has its name changed to fileToReplace and the old file is deleted.
	void ArchivePacker::ReplaceFileWithFile(const std::string& fileToReplace, const std::string& newFile)
	{
		// If C++17 filesystem comes out of experimental status on both MSVC and Linux compilers, 
		// consider switching to std::filesystem::rename & std::filesystem::remove
		  
		const std::string tempFileName("Temporary.vol");

		if (!std::rename(fileToReplace.c_str(), tempFileName.c_str())) {
			throw std::runtime_error("Unable to move original version of file to " + tempFileName);
		}
		
		if (!std::rename(newFile.c_str(), fileToReplace.c_str())) {
			// Attempt to restore original file.
			if (!std::rename(tempFileName.c_str(), fileToReplace.c_str())) {
				throw std::runtime_error("Unable to move new file into original filename. Attempting to restore original file failed. The original file may be found at " + tempFileName);
			}
			
			throw std::runtime_error("Unable to move new file into original version of filename. Original file was restored.");
		}
		
		if (!std::remove(tempFileName.c_str())) {
			throw std::runtime_error("Unable to delete temporary file named " + tempFileName + ". Consider manually deleting.");
		}
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
