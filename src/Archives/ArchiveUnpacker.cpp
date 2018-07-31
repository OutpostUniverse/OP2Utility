#include "ArchiveUnpacker.h"
#include "../XFile.h"
#include "../Streams/SeekableStreamReader.h"
#include "../Streams/FileStreamWriter.h"
#include <array>
#include <stdexcept>

namespace Archives
{
	ArchiveUnpacker::ArchiveUnpacker(const std::string& filename) :
		m_ArchiveFilename(filename), m_NumberOfPackedItems(0), m_ArchiveFileSize(0) { }

	ArchiveUnpacker::~ArchiveUnpacker() { }

	void ArchiveUnpacker::ExtractAllFiles(const std::string& destDirectory)
	{
		for (std::size_t i = 0; i < GetNumberOfPackedFiles(); ++i)
		{
			ExtractFile(i, XFile::ReplaceFilename(destDirectory, GetInternalName(i)));
		}
	}

	std::size_t ArchiveUnpacker::GetInternalItemIndex(const std::string& internalName)
	{
		for (std::size_t i = 0; i < GetNumberOfPackedFiles(); ++i)
		{
			if (XFile::PathsAreEqual(GetInternalName(i), internalName)) {
				return i;
			}
		}

		throw std::runtime_error("Archive " + m_ArchiveFilename + " does not contain " + internalName);
	}

	bool ArchiveUnpacker::ContainsItem(const std::string& internalName)
	{
		for (std::size_t i = 0; i < GetNumberOfPackedFiles(); ++i)
		{
			if (XFile::PathsAreEqual(GetInternalName(i), internalName)) {
				return true;
			}
		}

		return false;
	}

	void ArchiveUnpacker::ExtractFile(const std::string& internalFilename, const std::string& pathOut)
	{
		ExtractFile(GetInternalItemIndex(internalFilename), pathOut);
	}

	std::unique_ptr<SeekableStreamReader> ArchiveUnpacker::OpenStream(const std::string& internalName)
	{
		return OpenStream(GetInternalItemIndex(internalName));
	}

	void ArchiveUnpacker::CheckPackedIndexBounds(std::size_t index)
	{
		if (index >= m_NumberOfPackedItems) {
			throw std::runtime_error("Provided index is outside the bounds of packed items in archive " + m_ArchiveFilename + ".");
		}
	}
}
