#pragma once

#include <string>
#include <memory>

class SeekableStreamReader;

namespace Archives
{
	class ArchiveUnpacker
	{
	public:
		ArchiveUnpacker(const char *fileName);
		virtual ~ArchiveUnpacker();

		const char* GetVolumeFileName() { return m_VolumeFileName; };
		int GetVolumeFileSize() { return m_VolumeFileSize; };
		int GetNumberOfPackedFiles() { return m_NumberOfPackedFiles; };
		bool ContainsFile(const char* fileName);

		virtual const char* GetInternalFileName(int index) = 0;
		// Returns -1 if internalFileName is not present in archive.
		virtual int GetInternalFileIndex(const char *internalFileName) = 0;
		virtual int GetInternalFileSize(int index) = 0;
		virtual void ExtractFile(int fileIndex, const std::string& pathOut) = 0;
		virtual void ExtractAllFiles(const char* destDirectory);
		virtual std::unique_ptr<SeekableStreamReader> OpenSeekableStreamReader(const char *internalFileName) = 0;
		virtual std::unique_ptr<SeekableStreamReader> OpenSeekableStreamReader(int fileIndex) = 0;

	protected:
		char *m_VolumeFileName;
		int m_NumberOfPackedFiles;
		int m_VolumeFileSize;
	};
}
