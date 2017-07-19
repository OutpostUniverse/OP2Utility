#include "ResourceManager.h"

ResourceManager::ResourceManager(const string& archiveDirectory)
{
	vector<string> filenames;
	XFile::getFilesFromDirectory(filenames, archiveDirectory, ".vol");

	for (const string& volFilename : filenames)
		archiveFiles.push_back(CVolFile(volFilename.c_str()));

	filenames.clear();
	XFile::getFilesFromDirectory(filenames, archiveDirectory, ".clm");

	for (const string& clmFilename : filenames)
		archiveFiles.push_back(CClmFile(clmFilename.c_str()));
}

SeekableStreamReader* ResourceManager::getResourceStream(const string& filename)
{
	if (XFile::PathExists(filename))
	{
		return &FileStreamReader(filename);
	}

	for (CArchiveFile& archiveFile : volFiles)
	{
		int internalArchiveIndex = archiveFile.GetInternalFileIndex(filename.c_str());

		if (internalArchiveIndex > -1)
			return archiveFile.OpenSeekableStreamReader(internalArchiveIndex);
	}

	return nullptr;
}

bool ResourceManager::extractFromArchive(const string& filename, bool overwrite)
{
	if (!overwrite && XFile::PathExists(filename))
		return false;

	for (size_t i = 0; i < archiveFiles.size(); ++i)
	{
		for (int j = 0; j < archiveFiles[i].GetNumberOfPackedFiles(); ++j)
		{
			if (XFile::pathsAreEqual(archiveFiles[i].GetInternalFileName(j), filename))
			{
				archiveFiles[i].ExtractFile(j, filename.c_str());
				return true;
			}
		}
	}

	return false;
}

void ResourceManager::extractAllOfFileType(const string& directory, const string& extension, bool overwrite)
{
	for (size_t i = 0; i < archiveFiles.size(); ++i)
	{
		for (int j = 0; j < archiveFiles[i].GetNumberOfPackedFiles(); ++j)
		{
			if (XFile::extensionMatches(archiveFiles[i].GetInternalFileName(j), extension))
				archiveFiles[i].ExtractFile(i, archiveFiles[i].GetInternalFileName(j));
		}
	}
}