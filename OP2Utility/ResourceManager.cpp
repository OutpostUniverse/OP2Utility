#include "ResourceManager.h"

ResourceManager::ResourceManager(const string& archiveDirectory)
{
	vector<string> filenames;
	XFile::getFilesFromDirectory(filenames, archiveDirectory, ".vol");

	for each (const string& volFilename in filenames)
		archiveFiles.push_back(new CVolFile(volFilename.c_str()));

	filenames.clear();
	XFile::getFilesFromDirectory(filenames, archiveDirectory, ".clm");

	for each (const string& clmFilename in filenames)
		archiveFiles.push_back(new CClmFile(clmFilename.c_str()));
}

ResourceManager::~ResourceManager()
{
	for each (CArchiveFile* archiveFile in archiveFiles)
		delete archiveFile;
}

SeekableStreamReader* ResourceManager::getResourceStream(const string& filename)
{
	if (XFile::PathExists(filename))
		return new FileStreamReader(filename);

	for (CArchiveFile* archiveFile : archiveFiles)
	{
		int internalArchiveIndex = archiveFile->GetInternalFileIndex(filename.c_str());

		if (internalArchiveIndex > -1)
			return archiveFile->OpenSeekableStreamReader(internalArchiveIndex);
	}

	return nullptr;
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

bool ResourceManager::extractFromArchive(const string& filename, bool overwrite)
{
	if (!overwrite && XFile::PathExists(filename))
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
	for each (CVolFile* volFile in archiveFiles)
	{
		for (int i = 0; i < volFile->GetNumberOfPackedFiles(); ++i)
		{
			if (XFile::extensionMatches(volFile->GetInternalFileName(i), extension))
				volFile->ExtractFile(i, volFile->GetInternalFileName(i));
		}
	}
}