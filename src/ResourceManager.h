#pragma once

#include "Archive/ArchiveFile.h"
#include <string>
#include <vector>
#include <memory>
#include <cstddef>

namespace Stream {
	class BiDirectionalSeekableReader;
}

// Use to find files/resources either on disk or contained in an archive file (.vol|.clm).
class ResourceManager
{
public:
	ResourceManager(const std::string& archiveDirectory);

	std::unique_ptr<Stream::BiDirectionalSeekableReader> GetResourceStream(const std::string& filename, bool accessArchives = true);

	std::vector<std::string> GetAllFilenames(const std::string& directory, const std::string& filenameRegexStr, bool accessArchives = true);
	std::vector<std::string> GetAllFilenamesOfType(const std::string& directory, const std::string& extension, bool accessArchives = true);

	// Searches .vol and .clm archives for file and then extracts it.
	// returns true if EITHER the file is extracted OR
	//     if BOTH overwrite == false AND the file already exists in the directory.
	bool ExtractSpecificFile(const std::string& filename, bool overwrite = false);

	// Searches all .vol and .clm files and extracts any file with the given extension.
	void ExtractAllOfFileType(const std::string& directory, const std::string& extension, bool overwrite = false);

	// Returns an empty string if file is not located in an archive file in the ResourceManager's working directory.
	std::string FindContainingArchiveFile(const std::string& filename);

	// Returns a list of all loaded archives
	std::vector<std::string> GetArchiveFilenames();

private:
	std::vector<std::unique_ptr<Archive::ArchiveFile>> ArchiveFiles;

	bool ExistsInArchives(const std::string& filename, std::size_t& archiveIndexOut, std::size_t& internalIndexOut);
	bool DuplicateFilename(std::vector<std::string>& currentFilenames, std::string pathToCheck);
};
