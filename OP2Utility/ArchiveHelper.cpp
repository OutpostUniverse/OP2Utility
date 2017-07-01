#include "ArchiveHelper.h"
#include "XFile.h"
#include "StringHelper.h"
#include "Archives\CVolFile.h"
#include <vector>

void ArchiveHelper::extractAllOfFileType(const string& directory, const string& extension)
{
	vector<string> volFilenames;
	XFile::getFilesFromDirectory(volFilenames, directory, ".vol");

	for each (string volFilename in volFilenames)
	{
		CVolFile volFile(volFilename.c_str());

		for (int i = 0; i < volFile.GetNumberOfPackedFiles(); ++i)
		{
			if (XFile::extensionMatches(volFile.GetInternalFileName(i), extension))
				volFile.ExtractFile(i, volFile.GetInternalFileName(i));
		}
	}
}

void ArchiveHelper::findFile(const string& filename, bool accessArchives)
{
	if (XFile::PathExists(filename))
		return;

	if (!accessArchives)
		throw invalid_argument(filename + " Cannot be found. Consider enabling searching through VOL files.");

	bool fileFound = extractFileFromVolArchivesInDirectory(XFile::removeFilename(filename));

	if (!fileFound)
		throw invalid_argument(filename + " Cannot be found in specified directory or VOL files contained in directory.");
}

bool ArchiveHelper::extractFileFromVolArchivesInDirectory(string& filename)
{
	vector<string> volFilenames;
	XFile::getFilesFromDirectory(volFilenames, filename, ".vol");

	for each (string volFilename in volFilenames)
	{
		bool extracted = extractFileFromVolArchive(volFilename, filename);

		if (extracted)
			return true;
	}

	return false;
}

bool ArchiveHelper::extractFileFromVolArchive(const string& volFilename, const string& filename)
{
	CVolFile volFile(volFilename.c_str());

	for (int i = 0; i < volFile.GetNumberOfPackedFiles(); ++i)
	{
		if (XFile::pathsAreEqual(volFile.GetInternalFileName(i), filename))
		{
			volFile.ExtractFile(i, filename.c_str());
			return true;
		}
	}

	return false;
}