#pragma once

#include <string>
#include <memory>
#include <cstdint>
#include <cstddef>

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
		std::size_t GetNumberOfPackedFiles() { return m_NumberOfPackedItems; };
		bool ContainsItem(const std::string& filename);
		// Returns -1 if internalFilename is not present in archive.
		int GetInternalItemIndex(const std::string& internalName);
		void ExtractFile(const std::string& internalFilename, const std::string& pathOut);

		virtual std::string GetInternalName(std::size_t index) = 0;
		virtual uint32_t GetInternalItemSize(std::size_t index) = 0;
		virtual void ExtractFile(std::size_t index, const std::string& pathOut) = 0;
		virtual void ExtractAllFiles(const std::string& destDirectory);
		virtual std::unique_ptr<SeekableStreamReader> OpenStream(std::size_t index) = 0;
		virtual std::unique_ptr<SeekableStreamReader> OpenStream(const std::string& internalName);

	protected:
		void CheckPackedIndexBounds(std::size_t index);

		const std::string m_ArchiveFilename;
		std::size_t m_NumberOfPackedItems;
		uint64_t m_ArchiveFileSize;
	};
}
