#pragma once

#include <string>
#include <memory>
#include <cstdint>

class SeekableStreamReader;

namespace Archives
{
	class ArchiveUnpacker
	{
	public:
		ArchiveUnpacker(const std::string& fileName);
		virtual ~ArchiveUnpacker();

		std::string GetVolumeFileName() { return m_ArchiveFileName; };
		uint32_t GetVolumeFileSize() { return m_ArchiveFileSize; };
		int GetNumberOfPackedFiles() { return m_NumberOfPackedFiles; };
		bool ContainsFile(const char* fileName);

		virtual std::string GetInternalFileName(int index) = 0;
		// Returns -1 if internalFileName is not present in archive.
		virtual int GetInternalFileIndex(const char *internalFileName) = 0;
		virtual int GetInternalFileSize(int index) = 0;
		virtual void ExtractFile(int fileIndex, const std::string& pathOut) = 0;
		virtual void ExtractAllFiles(const char* destDirectory);
		virtual std::unique_ptr<SeekableStreamReader> OpenSeekableStreamReader(const char *internalFileName) = 0;
		virtual std::unique_ptr<SeekableStreamReader> OpenSeekableStreamReader(int fileIndex) = 0;

	protected:
		void CheckPackedFileIndexBounds(int fileIndex);

		std::string m_ArchiveFileName;
		int m_NumberOfPackedFiles;
		int m_ArchiveFileSize;
	};
}
