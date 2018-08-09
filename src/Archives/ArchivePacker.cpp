#include "ArchivePacker.h"
#include "../XFile.h"
#include "../StringHelper.h"
#include <cstddef>
#include <stdexcept>
#include <array>

namespace Archives
{
	ArchivePacker::ArchivePacker() { }
	ArchivePacker::~ArchivePacker() { }

	std::vector<std::string> ArchivePacker::GetInternalNamesFromPaths(const std::vector<std::string>& paths)
	{
		std::vector<std::string> filenames;

		for (const std::string& filename : paths) {
			filenames.push_back(XFile::GetFilename(filename));
		}

		return filenames;
	}

	void ArchivePacker::CheckSortedContainerForDuplicateNames(const std::vector<std::string>& internalNames)
	{
		for (std::size_t i = 1; i < internalNames.size(); ++i)
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