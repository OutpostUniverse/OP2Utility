#include "ArchiveUnpacker.h"
#include "../XFile.h"
#include "../Streams/SeekableStreamReader.h"
#include "../Streams/FileStreamWriter.h"
#include <array>
#include <stdexcept>

namespace Archives
{
	ArchiveUnpacker::ArchiveUnpacker(const std::string& filename) :
		m_ArchiveFilename(filename), m_PackedCount(0), m_ArchiveFileSize(0) { }

	ArchiveUnpacker::~ArchiveUnpacker() { }

	void ArchiveUnpacker::ExtractAllFiles(const std::string& destDirectory)
	{
		for (std::size_t i = 0; i < GetPackedCount(); ++i)
		{
			ExtractFile(i, XFile::ReplaceFilename(destDirectory, GetName(i)));
		}
	}

	std::size_t ArchiveUnpacker::GetIndex(const std::string& name)
	{
		for (std::size_t i = 0; i < GetPackedCount(); ++i)
		{
			if (XFile::PathsAreEqual(GetName(i), name)) {
				return i;
			}
		}

		throw std::runtime_error("Archive " + m_ArchiveFilename + " does not contain " + name);
	}

	bool ArchiveUnpacker::Contains(const std::string& name)
	{
		for (std::size_t i = 0; i < GetPackedCount(); ++i)
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

	std::unique_ptr<SeekableStreamReader> ArchiveUnpacker::OpenStream(const std::string& name)
	{
		return OpenStream(GetIndex(name));
	}

	void ArchiveUnpacker::CheckIndexBounds(std::size_t index)
	{
		if (index >= m_PackedCount) {
			throw std::runtime_error("Provided index is outside the bounds of packed items in archive " + m_ArchiveFilename + ".");
		}
	}
}
