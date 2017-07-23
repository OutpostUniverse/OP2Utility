#include "ResourceManager.h"

ResourceManager::ResourceManager(const string& archiveDirectory)
{
	vector<string> filenames;
	XFile::getFilesFromDirectory(filenames, archiveDirectory, ".vol");

	for each (const string& volFilename in filenames)
		archiveFiles.push_back(new VolFile(volFilename.c_str()));

	filenames.clear();
	XFile::getFilesFromDirectory(filenames, archiveDirectory, ".clm");

	for each (const string& clmFilename in filenames)
		archiveFiles.push_back(new ClmFile(clmFilename.c_str()));
}

ResourceManager::~ResourceManager()
{
	for each (ArchiveFile* archiveFile in archiveFiles)
		delete archiveFile;
}

// First searches for resources loosely in provided directory. 
// Then, if accessArhives = true, searches the preloaded archives for the resource.
SeekableStreamReader* ResourceManager::getResourceStream(const string& filename, bool accessArchives)
{
	if (XFile::PathExists(filename))
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

void ResourceManager::getAllStreamsOfFileType(vector<SeekableStreamReader*> seekableStreamReadersOut, const string& directory, const string& extension, bool accessArchives)
{
	vector<string> filenames;
	XFile::getFilesFromDirectory(filenames, directory, extension);
	
	for each (string filename in filenames)
		seekableStreamReadersOut.push_back(getResourceStream(filename, false));

	if (!accessArchives)
		return;

	for each (VolFile* volFile in archiveFiles)
	{
		for (int i = 0; i < volFile->GetNumberOfPackedFiles(); ++i)
		{
			if (XFile::extensionMatches(volFile->GetInternalFileName(i), extension))
				seekableStreamReadersOut.push_back(volFile->OpenSeekableStreamReader(i));
		}
	}
}

void ResourceManager::getAllFilenamesOfType(vector<string>& filenamesOut, const string& directory, const string& extension, bool accessArchives)
{
	XFile::getFilesFromDirectory(filenamesOut, directory, extension);

	if (!accessArchives)
		return;

	for each (VolFile* volFile in archiveFiles)
	{
		for (int i = 0; i < volFile->GetNumberOfPackedFiles(); ++i)
		{
			if (XFile::extensionMatches(volFile->GetInternalFileName(i), extension))
				filenamesOut.push_back(volFile->GetInternalFileName(i));
		}
	}
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
	for each (VolFile* volFile in archiveFiles)
	{
		for (int i = 0; i < volFile->GetNumberOfPackedFiles(); ++i)
		{
			if (XFile::extensionMatches(volFile->GetInternalFileName(i), extension))
				volFile->ExtractFile(i, volFile->GetInternalFileName(i));
		}
	}
}