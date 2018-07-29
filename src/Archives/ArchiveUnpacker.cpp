#include "ArchiveUnpacker.h"
#include "../XFile.h"
#include "../Streams/SeekableStreamReader.h"
#include "../Streams/FileStreamWriter.h"
#include <array>
#include <cstddef>
#include <stdexcept>

namespace Archives
{
	ArchiveUnpacker::ArchiveUnpacker(const std::string& filename) :
		m_ArchiveFilename(filename), m_NumberOfPackedItems(0), m_ArchiveFileSize(0) { }

	ArchiveUnpacker::~ArchiveUnpacker() { }

	void ArchiveUnpacker::ExtractAllFiles(const std::string& destDirectory)
	{
		for (int i = 0; i < GetNumberOfPackedFiles(); ++i)
		{
			ExtractFile(i, XFile::ReplaceFilename(destDirectory, GetInternalName(i)));
		}
	}

	int ArchiveUnpacker::GetInternalItemIndex(const std::string& internalFilename)
	{
		for (int i = 0; i < GetNumberOfPackedFiles(); ++i)
		{
			if (XFile::PathsAreEqual(GetInternalName(i), internalFilename)) {
				return i;
			}
		}

		return -1;
	}

	bool ArchiveUnpacker::ContainsItem(const std::string& internalName)
	{
		for (int i = 0; i < GetNumberOfPackedFiles(); ++i)
		{
			if (XFile::PathsAreEqual(GetInternalName(i), internalName)) {
				return true;
			}
		}

		return false;
	}

	void ArchiveUnpacker::ExtractFile(const std::string& internalFilename, const std::string& pathOut)
	{
		int index = GetInternalItemIndex(internalFilename);

		if (index == -1) {
			throw std::runtime_error("Archive " + m_ArchiveFilename + " does not contain a file named " + internalFilename);
		}

		ExtractFile(index, pathOut);
	}

	std::unique_ptr<SeekableStreamReader> ArchiveUnpacker::OpenStream(const std::string& internalName)
	{
		int index = GetInternalItemIndex(internalName);

		if (index == -1) {
			throw std::runtime_error("Archive " + m_ArchiveFilename + " does not contain a file named " + internalName);
		}

		return OpenStream(index);
	}

	void ArchiveUnpacker::CheckPackedIndexBounds(int index)
	{
		if (index < 0 || index >= m_NumberOfPackedItems) {
			throw std::runtime_error("Provided index is outside the bounds of packed items in archive " + m_ArchiveFilename + ".");
		}
	}
}
