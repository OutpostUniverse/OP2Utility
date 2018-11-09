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

		std::string GetVolumeFilename() const { return m_ArchiveFilename; };
		uint64_t GetVolumeFileSize() const { return m_ArchiveFileSize; };
		std::size_t GetCount() const { return m_Count; };
		bool Contains(const std::string& name);
		std::size_t GetIndex(const std::string& name);
		void ExtractFile(const std::string& name, const std::string& pathOut);

		virtual std::string GetName(std::size_t index) = 0;
		virtual uint32_t GetSize(std::size_t index) = 0;
		virtual void ExtractFile(std::size_t index, const std::string& pathOut) = 0;
		virtual void ExtractAllFiles(const std::string& destDirectory);
		virtual std::unique_ptr<SeekableStreamReader> OpenStream(std::size_t index) = 0;
		virtual std::unique_ptr<SeekableStreamReader> OpenStream(const std::string& name);

	protected:
		void CheckIndexBounds(std::size_t index);

		const std::string m_ArchiveFilename;
		std::size_t m_Count;
		uint64_t m_ArchiveFileSize;
	};
}
