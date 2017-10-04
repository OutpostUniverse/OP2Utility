#pragma once

#include "Archives/ArchiveFile.h"
#include <string>
#include <vector>
#include <regex>
#include <memory>

// Use to find files/resources either on disk or contained in an archive file (.vol|.clm).
class ResourceManager
{
public:
	ResourceManager(const std::string& archiveDirectory);

	std::unique_ptr<SeekableStreamReader> getResourceStream(const std::string& filename, bool accessArchives = true);

	std::vector<std::string> getAllFilenames(const std::string& directory, const std::string& filenameRegexStr, bool accessArcives = true);
	std::vector<std::string> getAllFilenamesOfType(const std::string& directory, const std::string& extension, bool accessArchives = true);

	// Searches .vol and .clm archives for file and then extracts it. 
	// returns true if EITHER the file is extracted OR 
	//     if BOTH overwrite == false AND the file already exists in the directory.
	bool extractSpecificFile(const std::string& filename, bool overwrite = false);

	// Searches all .vol and .clm files and extracts any file with the given extension.
	void extractAllOfFileType(const std::string& directory, const std::string& extension, bool overwrite = false);

	// Returns an empty string if file is not located in an archive file in the ResourceManager's working directory.
	std::string findContainingArchiveFile(const std::string& filename);

private:
	std::vector<std::unique_ptr<Archives::ArchiveFile>> archiveFiles;

	bool existsInArchives(const std::string& filename, int& volFileIndexOut, int& internalVolIndexOut);
	bool duplicateFilename(std::vector<std::string>& currentFilenames, std::string pathToCheck);
};
