#include "ArchiveFile.h"
#include "../XFile.h"
#include "../StringHelper.h"
#include "../Stream/BidirectionalReader.h"
#include "../Stream/FileWriter.h"
#include <array>
#include <string>
#include <stdexcept>

namespace Archive
{
	ArchiveFile::ArchiveFile(const std::string& filename) :
		m_ArchiveFilename(filename), m_Count(0), m_ArchiveFileSize(0) { }

	ArchiveFile::~ArchiveFile() { }

	void ArchiveFile::ExtractAllFiles(const std::string& destDirectory)
	{
		for (std::size_t i = 0; i < GetCount(); ++i)
		{
			ExtractFile(i, XFile::Append(destDirectory, GetName(i)));
		}
	}

	std::size_t ArchiveFile::GetIndex(const std::string& name)
	{
		for (std::size_t i = 0; i < GetCount(); ++i)
		{
			if (XFile::PathsAreEqual(GetName(i), name)) {
				return i;
			}
		}

		throw std::runtime_error("Archive " + m_ArchiveFilename + " does not contain " + name);
	}

	bool ArchiveFile::Contains(const std::string& name)
	{
		for (std::size_t i = 0; i < GetCount(); ++i)
		{
			if (XFile::PathsAreEqual(GetName(i), name)) {
				return true;
			}
		}

		return false;
	}

	void ArchiveFile::ExtractFile(const std::string& name, const std::string& pathOut)
	{
		ExtractFile(GetIndex(name), pathOut);
	}

	std::unique_ptr<Stream::BidirectionalReader> ArchiveFile::OpenStream(const std::string& name)
	{
		return OpenStream(GetIndex(name));
	}

	void ArchiveFile::VerifyIndexInBounds(std::size_t index)
	{
		if (index >= m_Count) {
			throw std::runtime_error("Index " + std::to_string(index) + " is out of bounds in archive " + m_ArchiveFilename + ".");
		}
	}


	std::vector<std::string> ArchiveFile::GetNamesFromPaths(const std::vector<std::string>& paths)
	{
		std::vector<std::string> filenames;

		for (const std::string& filename : paths) {
			filenames.push_back(XFile::GetFilename(filename));
		}

		return filenames;
	}

	void ArchiveFile::VerifySortedContainerHasNoDuplicateNames(const std::vector<std::string>& names)
	{
		for (std::size_t i = 1; i < names.size(); ++i)
		{
			if (StringHelper::CheckIfStringsAreEqual(names[i - 1], names[i])) {
				throw std::runtime_error("Unable to create an archive containing files with the same name. Duplicate name: " + names[i]);
			}
		}
	}

	bool ArchiveFile::ComparePathFilenames(const std::string path1, const std::string path2)
	{
		return StringHelper::StringCompareCaseInsensitive(XFile::GetFilename(path1), XFile::GetFilename(path2));
	}
}
