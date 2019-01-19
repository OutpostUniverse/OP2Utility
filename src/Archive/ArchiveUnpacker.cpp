#include "ArchiveUnpacker.h"
#include "../XFile.h"
#include "../Stream/BiDirectionalSeekableReader.h"
#include "../Stream/FileWriter.h"
#include <array>
#include <string>
#include <stdexcept>

namespace Archive
{
	ArchiveUnpacker::ArchiveUnpacker(const std::string& filename) :
		m_ArchiveFilename(filename), m_Count(0), m_ArchiveFileSize(0) { }

	ArchiveUnpacker::~ArchiveUnpacker() { }

	void ArchiveUnpacker::ExtractAllFiles(const std::string& destDirectory)
	{
		for (std::size_t i = 0; i < GetCount(); ++i)
		{
			ExtractFile(i, XFile::ReplaceFilename(destDirectory, GetName(i)));
		}
	}

	std::size_t ArchiveUnpacker::GetIndex(const std::string& name)
	{
		for (std::size_t i = 0; i < GetCount(); ++i)
		{
			if (XFile::PathsAreEqual(GetName(i), name)) {
				return i;
			}
		}

		throw std::runtime_error("Archive " + m_ArchiveFilename + " does not contain " + name);
	}

	bool ArchiveUnpacker::Contains(const std::string& name)
	{
		for (std::size_t i = 0; i < GetCount(); ++i)
		{
			if (XFile::PathsAreEqual(GetName(i), name)) {
				return true;
			}
		}

		return false;
	}

	void ArchiveUnpacker::ExtractFile(const std::string& name, const std::string& pathOut)
	{
		ExtractFile(GetIndex(name), pathOut);
	}

	std::unique_ptr<Stream::SeekableReader> ArchiveUnpacker::OpenStream(const std::string& name)
	{
		return OpenStream(GetIndex(name));
	}

	void ArchiveUnpacker::VerifyIndexInBounds(std::size_t index)
	{
		if (index >= m_Count) {
			throw std::runtime_error("Index " + std::to_string(index) + " is out of bounds in archive " + m_ArchiveFilename + ".");
		}
	}
}
