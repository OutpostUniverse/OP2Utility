#include "ResourceManager.h"
#include "XFile.h"
#include "Archives\VolFile.h"
#include "Archives\ClmFile.h"

ResourceManager::ResourceManager(const string& archiveDirectory)
{
	vector<string> volFilenames = XFile::getFilesFromDirectory(archiveDirectory, ".vol");

	for (const string& volFilename : volFilenames)
		archiveFiles.push_back(new VolFile(volFilename.c_str()));

	vector<string> clmFilenames = XFile::getFilesFromDirectory(archiveDirectory, ".clm");

	for (const string& clmFilename : clmFilenames)
		archiveFiles.push_back(new ClmFile(clmFilename.c_str()));
}

ResourceManager::~ResourceManager()
{
	for (ArchiveFile* archiveFile : archiveFiles)
		delete archiveFile;
}

// First searches for resources loosely in provided directory. 
// Then, if accessArhives = true, searches the preloaded archives for the resource.
SeekableStreamReader* ResourceManager::getResourceStream(const string& filename, bool accessArchives)
{
	if (XFile::pathExists(filename))
		return new FileStreamReader(filename);

	if (!accessArchives)
		return nullptr;

	for (ArchiveFile* archiveFile : archiveFiles)
	{
		string archiveFilename = XFile::getFilename(filename);
		int internalArchiveIndex = archiveFile->GetInternalFileIndex(archiveFilename.c_str());

		if (internalArchiveIndex > -1)
			return archiveFile->OpenSeekableStreamReader(internalArchiveIndex);
	}

	return nullptr;
}

vector<string> ResourceManager::getAllFilenames(const string& directory, const string& filenameRegexStr, bool accessArcives)
{
	regex filenameRegex(filenameRegexStr, regex_constants::icase);

	vector<string> filenames = XFile::getFilesFromDirectory(directory, filenameRegex);

	for (ArchiveFile* archiveFile : archiveFiles)
	{
		for (int i = 0; i < archiveFile->GetNumberOfPackedFiles(); ++i)
		{
			if (regex_search(archiveFile->GetInternalFileName(i), filenameRegex))
				filenames.push_back(archiveFile->GetInternalFileName(i));
		}
	}

	return filenames;
}

vector<string> ResourceManager::getAllFilenamesOfType(const string& directory, const string& extension, bool accessArchives)
{
	vector<string> filenames = XFile::getFilesFromDirectory(directory, extension);

	if (!accessArchives)
		return filenames;

	for (ArchiveFile* archiveFile : archiveFiles)
	{
		for (int i = 0; i < archiveFile->GetNumberOfPackedFiles(); ++i)
		{
			string internalFilename = archiveFile->GetInternalFileName(i);

			if (XFile::extensionMatches(internalFilename, extension) && !duplicateFilename(filenames, internalFilename))
				filenames.push_back(internalFilename);
		}
	}

	return filenames;
}

bool ResourceManager::existsInArchives(const string& filename, int& volFileIndexOut, int& internalVolIndexOut)
{
	for (size_t i = 0; i < archiveFiles.size(); ++i)
	{
		for (int j = 0; j < archiveFiles[i]->GetNumberOfPackedFiles(); ++j)
		{
			if (XFile::pathsAreEqual(archiveFiles[i]->GetInternalFileName(j), filename))
			{
				volFileIndexOut = i;
				internalVolIndexOut = j;
				return true;
			}
		}
	}

	return false;
}

bool ResourceManager::extractSpecificFile(const string& filename, bool overwrite)
{
	if (!overwrite && XFile::pathExists(filename))
		return true;

	int fileIndex;
	int internalArchiveIndex;
	if (existsInArchives(filename, fileIndex, internalArchiveIndex))
	{
		archiveFiles[fileIndex]->ExtractFile(internalArchiveIndex, filename.c_str());
		return true;
	}

	return false;
}

void ResourceManager::extractAllOfFileType(const string& directory, const string& extension, bool overwrite)
{
	for (ArchiveFile* archiveFile : archiveFiles)
	{
		for (int i = 0; i < archiveFile->GetNumberOfPackedFiles(); ++i)
		{
			if (XFile::extensionMatches(archiveFile->GetInternalFileName(i), extension))
				archiveFile->ExtractFile(i, archiveFile->GetInternalFileName(i));
		}
	}
}

bool ResourceManager::duplicateFilename(vector<string>& currentFilenames, string pathToCheck)
{
	// Brett: When called on a large loop of filenames (60 more more) this function, this will create a bottleneck.

	string filename = XFile::getFilename(pathToCheck);

	for (size_t i = 0; i < currentFilenames.size(); ++i)
		if (XFile::pathsAreEqual(XFile::getFilename(currentFilenames[i]), filename))
			return true;

	return false;
}

string ResourceManager::findContainingArchiveFile(const string& filename)
{
	for (ArchiveFile* archiveFile : archiveFiles)
	{
		int internalFileIndex = archiveFile->GetInternalFileIndex(filename.c_str());

		if (internalFileIndex != -1)
			return XFile::getFilename(archiveFile->GetVolumeFileName());
	}

	return string();
}