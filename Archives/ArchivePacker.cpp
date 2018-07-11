#include "ArchivePacker.h"
#include "../XFile.h"
#include "../StringHelper.h"
#include <cstddef>
#include <stdexcept>
#include <array>

namespace Archives
{
	const uint32_t ARCHIVE_WRITE_SIZE = 0x00020000;

	ArchivePacker::ArchivePacker() { }
	ArchivePacker::~ArchivePacker() { }

	std::vector<std::string> ArchivePacker::GetInternalNamesFromPaths(const std::vector<std::string>& paths)
	{
		std::vector<std::string> fileNames;

		for (const std::string& fileName : paths) {
			fileNames.push_back(XFile::GetFilename(fileName));
		}

		return fileNames;
	}

	void ArchivePacker::CheckSortedContainerForDuplicateNames(const std::vector<std::string>& internalNames) 
	{
		for (std::size_t i = 1; i < internalNames.size(); ++i)
		{
			if (StringHelper::CheckIfStringsAreEqual(internalNames[i - 1], internalNames[i])) {
				throw std::runtime_error("Unable to create an archive containing files with the same filename. Duplicate filename: " + internalNames[i]);
			}
		}
	}

	bool ArchivePacker::ComparePathFilenames(const std::string path1, const std::string path2)
	{
		return StringHelper::StringCompareCaseInsensitive(XFile::GetFilename(path1), XFile::GetFilename(path2));
	}

	void ArchivePacker::WriteFromStream(StreamWriter& streamWriter, StreamReader& streamReader, const uint64_t writeLength)
	{
		std::size_t numBytesToRead;
		uint32_t offset = 0;
		std::array<char, ARCHIVE_WRITE_SIZE> buffer;

		do
		{
			numBytesToRead = ARCHIVE_WRITE_SIZE;

			// Check if less than ARCHIVE_WRITE_SIZE of data remains for writing to disk.
			if (offset + numBytesToRead > writeLength) {
				numBytesToRead = static_cast<std::size_t>(writeLength - offset);
			}

			// Read the input file
			streamReader.Read(buffer.data(), numBytesToRead);
			offset += numBytesToRead;

			streamWriter.Write(buffer.data(), numBytesToRead);
		} while (numBytesToRead); // End loop when numBytesRead/Written is equal to 0
	}
}
