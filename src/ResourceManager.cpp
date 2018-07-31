#include "ResourceManager.h"
#include "Archives/VolFile.h"
#include "Archives/ClmFile.h"
#include "Streams/SeekableStreamReader.h"
#include "XFile.h"
#include <regex>

using namespace Archives;

ResourceManager::ResourceManager(const std::string& archiveDirectory)
{
	auto volFilenames = XFile::GetFilesFromDirectory(archiveDirectory, ".vol");

	for (const auto& volFilename : volFilenames) {
		ArchiveFiles.push_back(std::make_unique<VolFile>(volFilename));
	}

	auto clmFilenames = XFile::GetFilesFromDirectory(archiveDirectory, ".clm");

	for (const auto& clmFilename : clmFilenames) {
		ArchiveFiles.push_back(std::make_unique<ClmFile>(clmFilename));
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

	for (const auto& archiveFile : ArchiveFiles)
	{
		std::string internalArchiveFilename = XFile::GetFilename(filename);
		int internalArchiveIndex = archiveFile->GetInternalItemIndex(internalArchiveFilename);

		if (internalArchiveIndex > -1) {
			return archiveFile->OpenStream(internalArchiveIndex);
		}
	}

	return nullptr;
}

std::vector<std::string> ResourceManager::GetAllFilenames(const std::string& directory, const std::string& filenameRegexStr, bool accessArchives)
{
	std::regex filenameRegex(filenameRegexStr, std::regex_constants::icase);

	std::vector<std::string> filenames = XFile::GetFilesFromDirectory(directory, filenameRegex);

	for (const auto& archiveFile : ArchiveFiles)
	{
		for (std::size_t i = 0; i < archiveFile->GetNumberOfPackedFiles(); ++i)
		{
			if (std::regex_search(archiveFile->GetInternalName(i), filenameRegex)) {
				filenames.push_back(archiveFile->GetInternalName(i));
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

	for (const auto& archiveFile : ArchiveFiles)
	{
		for (std::size_t i = 0; i < archiveFile->GetNumberOfPackedFiles(); ++i)
		{
			std::string internalFilename = archiveFile->GetInternalName(i);

			if (XFile::ExtensionMatches(internalFilename, extension) && !DuplicateFilename(filenames, internalFilename)) {
				filenames.push_back(internalFilename);
			}
		}
	}

	return filenames;
}

bool ResourceManager::ExistsInArchives(const std::string& filename, int& volFileIndexOut, size_t& internalVolIndexOut)
{
	for (std::size_t i = 0; i < ArchiveFiles.size(); ++i)
	{
		for (std::size_t j = 0; j < ArchiveFiles[i]->GetNumberOfPackedFiles(); ++j)
		{
			if (XFile::PathsAreEqual(ArchiveFiles[i]->GetInternalName(j), filename))
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
	std::size_t internalArchiveIndex;
	if (ExistsInArchives(filename, fileIndex, internalArchiveIndex))
	{
		ArchiveFiles[fileIndex]->ExtractFile(internalArchiveIndex, filename);
		return true;
	}

	return false;
}

void ResourceManager::ExtractAllOfFileType(const std::string& directory, const std::string& extension, bool overwrite)
{
	for (const auto& archiveFile : ArchiveFiles)
	{
		for (std::size_t i = 0; i < archiveFile->GetNumberOfPackedFiles(); ++i)
		{
			if (XFile::ExtensionMatches(archiveFile->GetInternalName(i), extension)) {
				archiveFile->ExtractFile(i, archiveFile->GetInternalName(i));
			}
		}
	}
}

bool ResourceManager::DuplicateFilename(std::vector<std::string>& currentFilenames, std::string pathToCheck)
{
	// Brett: When called on a large loop of filenames (60 or more), this function will create a bottleneck.

	std::string filename = XFile::GetFilename(pathToCheck);

	for (const auto& currentFilename : currentFilenames) {
		if (XFile::PathsAreEqual(XFile::GetFilename(currentFilename), filename)) {
			return true;
		}
	}

	return false;
}

std::string ResourceManager::FindContainingArchiveFile(const std::string& filename)
{
	for (const auto& archiveFile : ArchiveFiles)
	{
		int internalFileIndex = archiveFile->GetInternalItemIndex(filename);

		if (internalFileIndex != -1) {
			return XFile::GetFilename(archiveFile->GetVolumeFilename());
		}
	}

	return std::string();
}
