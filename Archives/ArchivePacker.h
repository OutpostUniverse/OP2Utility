#pragma once

#include "../Streams/StreamReader.h"
#include "../Streams/StreamWriter.h"
#include <string>
#include <vector>

namespace Archives
{
	class ArchivePacker
	{
	public:
		ArchivePacker();
		virtual ~ArchivePacker();

		// Repack is used to replace the old volume with a new volume created from the
		// files (in the current directory) that match the internal file names
		virtual bool Repack() = 0;
		// Create volume is used to create a new volume file with the files specified in filesToPack.
		// Returns true if successful and false otherwise
		virtual bool CreateArchive(std::string volumeFileName, std::vector<std::string> filesToPack) = 0;

	protected:
		// Returns the filenames from each path stripping the rest of the path. 
		std::vector<std::string> GetInternalNamesFromPaths(const std::vector<std::string>& paths);

		// Throws an error if 2 internalNames are identical, case insensitve. 
		// internalNames must be presorted. 
		void CheckSortedContainerForDuplicateNames(const std::vector<std::string>& internalNames);

		// Compares 2 filenames case insensitive to determine which comes first alphabetically.
		// Does not compare the entire path, but only the filename.
		static bool ComparePathFilenames(const std::string path1, const std::string path2);

		void WriteFromStream(StreamWriter& streamWriter, StreamReader& streamReader, const uint64_t writeLength);
	};
}
