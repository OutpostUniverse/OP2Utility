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
		m_ArchiveFilename(filename), m_NumberOfPackedFiles(0), m_ArchiveFileSize(0) { }

	ArchiveUnpacker::~ArchiveUnpacker() { }

	void ArchiveUnpacker::ExtractAllFiles(const std::string& destDirectory)
	{
		for (int i = 0; i < GetNumberOfPackedFiles(); ++i)
		{
			ExtractFile(i, XFile::ReplaceFilename(destDirectory, GetInternalFilename(i)));
		}
	}

	int ArchiveUnpacker::GetInternalFileIndex(const std::string& internalFilename)
	{
		for (int i = 0; i < GetNumberOfPackedFiles(); ++i)
		{
			if (XFile::PathsAreEqual(GetInternalFilename(i), internalFilename)) {
				return i;
			}
		}

		return -1;
	}

	bool ArchiveUnpacker::ContainsFile(const std::string& filename)
	{
		for (int i = 0; i < GetNumberOfPackedFiles(); ++i)
		{
			if (XFile::PathsAreEqual(GetInternalFilename(i), filename)) {
				return true;
			}
		}

		return false;
	}

	void ArchiveUnpacker::ExtractFile(const std::string& internalFilename, const std::string& pathOut)
	{
		int index = GetInternalFileIndex(internalFilename);

		if (index == -1) {
			throw std::runtime_error("Archive " + m_ArchiveFilename + " does not contain a file named " + internalFilename);
		}

		ExtractFile(index, pathOut);
	}

	std::unique_ptr<SeekableStreamReader> ArchiveUnpacker::OpenStream(const std::string& internalFilename)
	{
		int fileIndex = GetInternalFileIndex(internalFilename);

		if (fileIndex == -1) {
			throw std::runtime_error("Archive " + m_ArchiveFilename + " does not contain a file named " + internalFilename);
		}

		return OpenStream(fileIndex);
	}

	void ArchiveUnpacker::CheckPackedFileIndexBounds(int fileIndex)
	{
		if (fileIndex < 0 || fileIndex >= m_NumberOfPackedFiles) {
			throw std::runtime_error("fileIndex is outside the bounds of packed files.");
		}
	}
}
