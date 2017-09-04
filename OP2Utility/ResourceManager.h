#pragma once

#include "Archives/ArchiveFile.h"
#include <string>
#include <vector>
#include <regex>

using namespace std;
using namespace Archives;

// Use to find files/resources either on disk or contained in an archive file (.vol|.clm).
class ResourceManager
{
public:
	ResourceManager(const string& archiveDirectory);
	~ResourceManager();

	SeekableStreamReader* getResourceStream(const string& filename, bool accessArchives = true);

	vector<string> getAllFilenames(const string& directory, const string& filenameRegexStr, bool accessArcives = true);
	vector<string> getAllFilenamesOfType(const string& directory, const string& extension, bool accessArchives = true);

	// Searches .vol and .clm archives for file and then extracts it. 
	// returns true if EITHER the file is extracted OR 
	//     if BOTH overwrite == false AND the file already exists in the directory.
	bool extractFileSpecificArchive(const string& filename, bool overwrite = false);

	// Searches all .vol and .clm files and extracts any file with the given extension.
	void extractAllOfFileType(const string& directory, const string& extension, bool overwrite = false);

	// Returns an empty string if file is not located in an archive file in the ResourceManager's working directory.
	string findContainingArchiveFile(const string& filename);

private:
	vector<ArchiveFile*> archiveFiles;

	bool existsInArchives(const string& filename, int& volFileIndexOut, int& internalVolIndexOut);
	bool duplicateFilename(vector<string>& currentFilenames, string pathToCheck);
};
