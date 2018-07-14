#include "ArchiveUnpacker.h"
#include "../XFile.h"
#include "../Streams/SeekableStreamReader.h"
#include "../Streams/FileStreamWriter.h"
#include <array>
#include <cstddef>

namespace Archives
{
	const std::size_t ARCHIVE_WRITE_SIZE = 0x00020000;

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

	void ArchiveUnpacker::CheckPackedFileIndexBounds(int fileIndex)
	{
		if (fileIndex < 0 || fileIndex >= m_NumberOfPackedFiles) {
			throw std::runtime_error("fileIndex is outside the bounds of packed files.");
		}
	}

	void ArchiveUnpacker::WriteFromStream(const std::string& filenameOut, StreamReader& streamReader, uint64_t writeLength)
	{
		FileStreamWriter fileStreamWriter(filenameOut);
		WriteFromStream(fileStreamWriter, streamReader, writeLength);
	}

	void ArchiveUnpacker::WriteFromStream(StreamWriter& streamWriter, StreamReader& streamReader, uint64_t writeLength)
	{
		std::size_t numBytesToRead = 0;
		uint64_t offset = 0;
		std::array<char, ARCHIVE_WRITE_SIZE> buffer;

		do
		{
			numBytesToRead = ARCHIVE_WRITE_SIZE;

			// Check if less than CLM_WRITE_SIZE of data remains for writing to disk.
			if (offset + numBytesToRead > writeLength) {
				numBytesToRead = static_cast<std::size_t>(writeLength - offset);
			}

			streamReader.Read(buffer.data(), numBytesToRead);

			offset += numBytesToRead;

			streamWriter.Write(buffer.data(), numBytesToRead);
		} while (numBytesToRead); // End loop when numBytesRead/Written is equal to 0
	}
}
