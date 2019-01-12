#pragma once

#include "Archives/ArchiveFile.h"
#include <string>
#include <vector>
#include <memory>
#include <cstddef>

namespace Stream {
	class SeekableReader;
}

// Use to find files/resources either on disk or contained in an archive file (.vol|.clm).
class ResourceManager
{
public:
	ResourceManager(const std::string& archiveDirectory);

	std::unique_ptr<Stream::SeekableReader> GetResourceStream(const std::string& filename, bool accessArchives = true);

	std::vector<std::string> GetAllFilenames(const std::string& filenameRegexStr, bool accessArchives = true);
	std::vector<std::string> GetAllFilenamesOfType(const std::string& extension, bool accessArchives = true);

	// Returns an empty string if file is not located in an archive file in the ResourceManager's working directory.
	std::string FindContainingArchive(const std::string& filename);

	// Returns a list of all loaded archives
	std::vector<std::string> GetArchiveFilenames();

private:
	const std::string directory;
	std::vector<std::unique_ptr<Archives::ArchiveFile>> ArchiveFiles;

	bool ExistsInArchives(const std::string& filename, std::size_t& archiveIndexOut, std::size_t& internalIndexOut);
	bool IsDuplicateFilename(std::vector<std::string>& currentFilenames, std::string filenameToCheck);
};
