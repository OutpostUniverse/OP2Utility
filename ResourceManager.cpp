#include "ResourceManager.h"
#include "XFile.h"
#include "Archives/VolFile.h"
#include "Archives/ClmFile.h"

using namespace std;
using namespace Archives;

ResourceManager::ResourceManager(const string& archiveDirectory)
{
	vector<string> volFilenames = XFile::GetFilesFromDirectory(archiveDirectory, ".vol");

	for (const string& volFilename : volFilenames)
		ArchiveFiles.push_back(make_unique<VolFile>(volFilename.c_str()));

	vector<string> clmFilenames = XFile::GetFilesFromDirectory(archiveDirectory, ".clm");

	for (const string& clmFilename : clmFilenames)
		ArchiveFiles.push_back(make_unique<ClmFile>(clmFilename.c_str()));
}

// First searches for resources loosely in provided directory. 
// Then, if accessArhives = true, searches the preloaded archives for the resource.
unique_ptr<SeekableStreamReader> ResourceManager::GetResourceStream(const string& filename, bool accessArchives)
{
	if (XFile::PathExists(filename))
		return make_unique<FileStreamReader>(filename);

	if (!accessArchives)
		return nullptr;

	for (unique_ptr<ArchiveFile>& archiveFile : ArchiveFiles)
	{
		string internalArchiveFilename = XFile::GetFilename(filename);
		int internalArchiveIndex = archiveFile->GetInternalFileIndex(internalArchiveFilename.c_str());

		if (internalArchiveIndex > -1)
			return archiveFile->OpenSeekableStreamReader(internalArchiveIndex);
	}

	return nullptr;
}

vector<string> ResourceManager::GetAllFilenames(const string& directory, const string& filenameRegexStr, bool accessArcives)
{
	regex filenameRegex(filenameRegexStr, regex_constants::icase);

	vector<string> filenames = XFile::GetFilesFromDirectory(directory, filenameRegex);

	for (unique_ptr<ArchiveFile>& archiveFile : ArchiveFiles)
	{
		for (int i = 0; i < archiveFile->GetNumberOfPackedFiles(); ++i)
		{
			if (regex_search(archiveFile->GetInternalFileName(i), filenameRegex))
				filenames.push_back(archiveFile->GetInternalFileName(i));
		}
	}

	return filenames;
}

vector<string> ResourceManager::GetAllFilenamesOfType(const string& directory, const string& extension, bool accessArchives)
{
	vector<string> filenames = XFile::GetFilesFromDirectory(directory, extension);

	if (!accessArchives)
		return filenames;

	for (unique_ptr<ArchiveFile>& archiveFile : ArchiveFiles)
	{
		for (int i = 0; i < archiveFile->GetNumberOfPackedFiles(); ++i)
		{
			string internalFilename = archiveFile->GetInternalFileName(i);

			if (XFile::ExtensionMatches(internalFilename, extension) && !DuplicateFilename(filenames, internalFilename))
				filenames.push_back(internalFilename);
		}
	}

	return filenames;
}

bool ResourceManager::ExistsInArchives(const string& filename, int& volFileIndexOut, int& internalVolIndexOut)
{
	for (size_t i = 0; i < ArchiveFiles.size(); ++i)
	{
		for (int j = 0; j < ArchiveFiles[i]->GetNumberOfPackedFiles(); ++j)
		{
			if (XFile::PathsAreEqual(ArchiveFiles[i]->GetInternalFileName(j), filename))
			{
				volFileIndexOut = i;
				internalVolIndexOut = j;
				return true;
			}
		}
	}

	return false;
}

bool ResourceManager::ExtractSpecificFile(const string& filename, bool overwrite)
{
	if (!overwrite && XFile::PathExists(filename))
		return true;

	int fileIndex;
	int internalArchiveIndex;
	if (ExistsInArchives(filename, fileIndex, internalArchiveIndex))
	{
		ArchiveFiles[fileIndex]->ExtractFile(internalArchiveIndex, filename.c_str());
		return true;
	}

	return false;
}

void ResourceManager::ExtractAllOfFileType(const string& directory, const string& extension, bool overwrite)
{
	for (unique_ptr<ArchiveFile>& archiveFile : ArchiveFiles)
	{
		for (int i = 0; i < archiveFile->GetNumberOfPackedFiles(); ++i)
		{
			if (XFile::ExtensionMatches(archiveFile->GetInternalFileName(i), extension))
				archiveFile->ExtractFile(i, archiveFile->GetInternalFileName(i));
		}
	}
}

bool ResourceManager::DuplicateFilename(vector<string>& currentFilenames, string pathToCheck)
{
	// Brett: When called on a large loop of filenames (60 more more) this function, this will create a bottleneck.

	string filename = XFile::GetFilename(pathToCheck);

	for (size_t i = 0; i < currentFilenames.size(); ++i)
		if (XFile::PathsAreEqual(XFile::GetFilename(currentFilenames[i]), filename))
			return true;

	return false;
}

string ResourceManager::FindContainingArchiveFile(const string& filename)
{
	for (unique_ptr<ArchiveFile>& archiveFile : ArchiveFiles)
	{
		int internalFileIndex = archiveFile->GetInternalFileIndex(filename.c_str());

		if (internalFileIndex != -1)
			return XFile::GetFilename(archiveFile->GetVolumeFileName());
	}

	return string();
}
