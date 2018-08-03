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

	std::vector<std::string> ArchivePacker::GetNamesFromPaths(const std::vector<std::string>& paths)
	{
		std::vector<std::string> filenames;

		for (const std::string& filename : paths) {
			filenames.push_back(XFile::GetFilename(filename));
		}

		return filenames;
	}

	void ArchivePacker::CheckSortedContainerForDuplicateNames(const std::vector<std::string>& names)
	{
		for (std::size_t i = 1; i < names.size(); ++i)
		{
			if (StringHelper::CheckIfStringsAreEqual(names[i - 1], names[i])) {
				throw std::runtime_error("Unable to create an archive containing files with the same name. Duplicate name: " + names[i]);
			}
		}
	}

	bool ArchivePacker::ComparePathFilenames(const std::string path1, const std::string path2)
	{
		return StringHelper::StringCompareCaseInsensitive(XFile::GetFilename(path1), XFile::GetFilename(path2));
	}
}
