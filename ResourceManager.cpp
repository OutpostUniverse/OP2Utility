#include "ResourceManager.h"
#include "Archives/VolFile.h"
#include "Archives/ClmFile.h"
#include "Streams/SeekableStreamReader.h"
#include "XFile.h"

using namespace Archives;

ResourceManager::ResourceManager(const std::string& archiveDirectory)
{
	std::vector<std::string> volFilenames = XFile::GetFilesFromDirectory(archiveDirectory, ".vol");

	for (const auto& volFilename : volFilenames) {
		ArchiveFiles.push_back(std::make_unique<VolFile>(volFilename.c_str()));
	}

	std::vector<std::string> clmFilenames = XFile::GetFilesFromDirectory(archiveDirectory, ".clm");

	for (const auto& clmFilename : clmFilenames) {
		ArchiveFiles.push_back(std::make_unique<ClmFile>(clmFilename.c_str()));
	}
}

// First searches for resources loosely in provided directory. 
// Then, if accessArhives = true, searches the preloaded archives for the resource.
std::unique_ptr<SeekableStreamReader> ResourceManager::GetResourceStream(const std::string& filename, bool accessArchives)
{
	if (XFile::PathExists(filename)) {
		return std::make_unique<FileStreamReader>(filename);
	}

	if (!accessArchives) {
		return nullptr;
	}

	for (auto& archiveFile : ArchiveFiles)
	{
		std::string internalArchiveFilename = XFile::GetFilename(filename);
		int internalArchiveIndex = archiveFile->GetInternalFileIndex(internalArchiveFilename.c_str());

		if (internalArchiveIndex > -1) {
			return archiveFile->OpenSeekableStreamReader(internalArchiveIndex);
		}
	}

	return nullptr;
}

std::vector<std::string> ResourceManager::GetAllFilenames(const std::string& directory, const std::string& filenameRegexStr, bool accessArcives)
{
	std::regex filenameRegex(filenameRegexStr, std::regex_constants::icase);

	std::vector<std::string> filenames = XFile::GetFilesFromDirectory(directory, filenameRegex);

	for (auto& archiveFile : ArchiveFiles)
	{
		for (int i = 0; i < archiveFile->GetNumberOfPackedFiles(); ++i)
		{
			if (std::regex_search(archiveFile->GetInternalFileName(i), filenameRegex)) {
				filenames.push_back(archiveFile->GetInternalFileName(i));
			}
		}
	}

	return filenames;
}

std::vector<std::string> ResourceManager::GetAllFilenamesOfType(const std::string& directory, const std::string& extension, bool accessArchives)
{
	std::vector<std::string> filenames = XFile::GetFilesFromDirectory(directory, extension);

	if (!accessArchives) {
		return filenames;
	}

	for (std::unique_ptr<ArchiveFile>& archiveFile : ArchiveFiles)
	{
		for (int i = 0; i < archiveFile->GetNumberOfPackedFiles(); ++i)
		{
			std::string internalFilename = archiveFile->GetInternalFileName(i);

			if (XFile::ExtensionMatches(internalFilename, extension) && !DuplicateFilename(filenames, internalFilename)) {
				filenames.push_back(internalFilename);
			}
		}
	}

	return filenames;
}

bool ResourceManager::ExistsInArchives(const std::string& filename, int& volFileIndexOut, int& internalVolIndexOut)
{
	for (std::size_t i = 0; i < ArchiveFiles.size(); ++i)
	{
		for (int j = 0; j < ArchiveFiles[i]->GetNumberOfPackedFiles(); ++j)
		{
			if (XFile::PathsAreEqual(ArchiveFiles[i]->GetInternalFileName(j), filename))
			{
				volFileIndexOut = static_cast<int>(i);
				internalVolIndexOut = j;
				return true;
			}
		}
	}

	return false;
}

bool ResourceManager::ExtractSpecificFile(const std::string& filename, bool overwrite)
{
	if (!overwrite && XFile::PathExists(filename)) {
		return true;
	}

	int fileIndex;
	int internalArchiveIndex;
	if (ExistsInArchives(filename, fileIndex, internalArchiveIndex))
	{
		ArchiveFiles[fileIndex]->ExtractFile(internalArchiveIndex, filename.c_str());
		return true;
	}

	return false;
}

void ResourceManager::ExtractAllOfFileType(const std::string& directory, const std::string& extension, bool overwrite)
{
	for (auto& archiveFile : ArchiveFiles)
	{
		for (int i = 0; i < archiveFile->GetNumberOfPackedFiles(); ++i)
		{
			if (XFile::ExtensionMatches(archiveFile->GetInternalFileName(i), extension)) {
				archiveFile->ExtractFile(i, archiveFile->GetInternalFileName(i));
			}
		}
	}
}

bool ResourceManager::DuplicateFilename(std::vector<std::string>& currentFilenames, std::string pathToCheck)
{
	// Brett: When called on a large loop of filenames (60 or more), this function will create a bottleneck.

	std::string filename = XFile::GetFilename(pathToCheck);

	for (std::size_t i = 0; i < currentFilenames.size(); ++i) {
		if (XFile::PathsAreEqual(XFile::GetFilename(currentFilenames[i]), filename)) {
			return true;
		}
	}

	return false;
}

std::string ResourceManager::FindContainingArchiveFile(const std::string& filename)
{
	for (auto& archiveFile : ArchiveFiles)
	{
		int internalFileIndex = archiveFile->GetInternalFileIndex(filename.c_str());

		if (internalFileIndex != -1) {
			return XFile::GetFilename(archiveFile->GetVolumeFileName());
		}
	}

	return std::string();
}
