#pragma once

#include "../Stream/BidirectionalReader.h"
#include <string>
#include <memory>
#include <cstdint>
#include <cstddef>

namespace Archive
{
	class ArchiveFile
	{
	public:
		ArchiveFile(const std::string& filename);
		virtual ~ArchiveFile();

		std::string GetArchiveFilename() const { return m_ArchiveFilename; };
		uint64_t GetArchiveFileSize() const { return m_ArchiveFileSize; };
		std::size_t GetCount() const { return m_Count; };
		bool Contains(const std::string& name);
		void ExtractFile(const std::string& name, const std::string& pathOut);

		virtual std::size_t GetIndex(const std::string& name);
		virtual std::string GetName(std::size_t index) = 0;
		virtual uint32_t GetSize(std::size_t index) = 0;
		virtual void ExtractFile(std::size_t index, const std::string& pathOut) = 0;
		virtual void ExtractAllFiles(const std::string& destDirectory);
		virtual std::unique_ptr<Stream::BidirectionalReader> OpenStream(std::size_t index) = 0;
		virtual std::unique_ptr<Stream::BidirectionalReader> OpenStream(const std::string& name);

	protected:
		void VerifyIndexInBounds(std::size_t index);

		// Returns the filenames from each path stripping the rest of the path.
		static std::vector<std::string> GetNamesFromPaths(const std::vector<std::string>& paths);

		// Throws an error if 2 names are identical, case insensitve.
		// names must be presorted.
		static void VerifySortedContainerHasNoDuplicateNames(const std::vector<std::string>& names);

		// Compares 2 filenames case insensitive to determine which comes first alphabetically.
		// Does not compare the entire path, but only the filename.
		static bool ComparePathFilenames(const std::string path1, const std::string path2);

		const std::string m_ArchiveFilename;
		std::size_t m_Count;
		uint64_t m_ArchiveFileSize;
	};
}
