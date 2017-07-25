#pragma once

#include <string>
#include <vector>
#include "OP2Utility.h"
#include "Archives/ArchiveFile.h"

using namespace std;
using namespace Archives;

// Use to find files/resources either on disk or contained in an archive file (.vol|.clm).
class ResourceManager
{
public:
	ResourceManager(const string& archiveDirectory);
	~ResourceManager();

	SeekableStreamReader* getResourceStream(const string& filename, bool accessArchives = true);

	void ResourceManager::getAllStreamsOfFileType(vector<SeekableStreamReader*> seekableStreamReadersOut, const string& directory, const string& extension, bool accessArchives = true);

	void ResourceManager::getAllFilenamesOfType(vector<string>& filenamesOut, const string& directory, const string& extension, bool accessArchives = true);

	// Searches .vol and .clm files for file and then extracts it. 
	// returns true if EITHER the file is extracted OR 
	//     if BOTH overwrite == false AND the file already exists in the directory.
	bool extractFromArchive(const string& filename, bool overwrite = false);

	// Searches all .vol and .clm files and extracts any file with the given extension.
	void extractAllOfFileType(const string& directory, const string& extension, bool overwrite = false);

	bool existsInArchives(const string& filename, int& volFileIndexOut, int& internalVolIndexOut);

	// Prevents SGAME10.OP2 from being returned in a directory wide file pull. Defaults to true.
	void setIgnoreSGame10(bool ignore) { ignoreSGame10 = ignore; };

	// Prevents wellpallet.bmp from being returned in a directory wide file pull. Defaults to true.
	void setIgnoreWellPallet(bool ignore) { ignoreWellPallet = ignore; };

private:
	bool ignoreSGame10 = true; //SGAME10.OP2
	bool ignoreWellPallet = true; //wellpallet.bmp
	vector<ArchiveFile*> archiveFiles;

	bool ignoreFilename(string filename);
	bool duplicateFilename(vector<string>& currentFilenames, string pathToCheck);
};
