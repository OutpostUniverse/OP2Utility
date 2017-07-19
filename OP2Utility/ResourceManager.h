#pragma once

#include <string>
#include <vector>
#include "StreamReader.h"
#include "Archives\CVolFile.h"
#include "Archives\CClmFile.h"
#include "Archives\CArchiveFile.h"
#include "XFile.h"
#include "StringHelper.h"

using namespace std;

// Use to find files/resources either on disk or contained in an archive file (.vol|.clm).
class ResourceManager
{
public:
	ResourceManager(const string& archiveDirectory);

	SeekableStreamReader* getResourceStream(const string& filename);
	


	// Searches .vol and .clm files for file and then extracts it. 
	// returns true if EITHER the file is extracted OR 
	//     if BOTH overwrite == false AND the file already exists in the directory.
	bool extractFromArchive(const string& filename, bool overwrite = false);

	// Searches all .vol and .clm files and extracts any file with the given extension.
	void extractAllOfFileType(const string& directory, const string& extension, bool overwrite = false);

private:
	vector<CArchiveFile> archiveFiles;
	vector<CVolFile> volFiles;
	vector<CClmFile> clmFiles;
};
