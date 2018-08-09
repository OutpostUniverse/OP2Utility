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
		ArchiveUnpacker(const std::string& filename);
		virtual ~ArchiveUnpacker();

		std::string GetVolumeFilename() { return m_ArchiveFilename; };
		uint64_t GetVolumeFileSize() { return m_ArchiveFileSize; };
		int GetNumberOfPackedFiles() { return m_NumberOfPackedFiles; };
		bool ContainsFile(const std::string& filename);
		// Returns -1 if internalFilename is not present in archive.
		int GetInternalFileIndex(const std::string& internalFilename);
		void ExtractFile(const std::string& internalFilename, const std::string& pathOut);

		virtual std::string GetInternalFilename(int index) = 0;
		virtual uint32_t GetInternalFileSize(int index) = 0;
		virtual void ExtractFile(int fileIndex, const std::string& pathOut) = 0;
		virtual void ExtractAllFiles(const std::string& destDirectory);
		virtual std::unique_ptr<SeekableStreamReader> OpenStream(int fileIndex) = 0;
		virtual std::unique_ptr<SeekableStreamReader> OpenStream(const std::string& internalFilename);

	protected:
		void CheckPackedFileIndexBounds(int fileIndex);

		const std::string m_ArchiveFilename;
		int m_NumberOfPackedFiles;
		uint64_t m_ArchiveFileSize;
	};
}