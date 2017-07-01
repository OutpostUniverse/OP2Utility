#pragma once

#include<string>

using namespace std;

class ArchiveHelper
{
public:
	static void findFile(const string& filename, bool accessArchives);
	static bool extractFileFromVolArchivesInDirectory(string& filename);
	static bool extractFileFromVolArchive(const string& volFilename, const string& filename);
	static void ArchiveHelper::extractAllOfFileType(const string& directory, const string& extension);
};
