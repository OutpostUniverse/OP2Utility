#pragma once

#include <string>
#include <memory>
#include <cstdint>

class SeekableStreamReader;
class StreamReader;
class StreamWriter;

namespace Archives
{
	class ArchiveUnpacker
	{
	public:
		ArchiveUnpacker(const std::string& fileName);
		virtual ~ArchiveUnpacker();

		std::string GetVolumeFileName() { return m_ArchiveFileName; };
		uint64_t GetVolumeFileSize() { return m_ArchiveFileSize; };
		int GetNumberOfPackedFiles() { return m_NumberOfPackedFiles; };
		bool ContainsFile(const std::string& fileName);

		virtual std::string GetInternalFileName(int index) = 0;
		// Returns -1 if internalFileName is not present in archive.
		virtual int GetInternalFileIndex(const std::string& internalFileName) = 0;
		virtual uint32_t GetInternalFileSize(int index) = 0;
		virtual void ExtractFile(int fileIndex, const std::string& pathOut) = 0;
		virtual void ExtractAllFiles(const std::string& destDirectory);
		virtual std::unique_ptr<SeekableStreamReader> OpenSeekableStreamReader(const std::string& internalFileName) = 0;
		virtual std::unique_ptr<SeekableStreamReader> OpenSeekableStreamReader(int fileIndex) = 0;

	protected:
		void CheckPackedFileIndexBounds(int fileIndex);
		void WriteFromStream(const std::string& filenameOut, StreamReader& streamReader, uint64_t writeLength);
		void WriteFromStream(StreamWriter& streamWriter, StreamReader& streamReader, uint64_t writeLength);

		const std::string m_ArchiveFileName;
		int m_NumberOfPackedFiles;
		uint64_t m_ArchiveFileSize;
	};
}
