#pragma once

#include <Windows.h>
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
		virtual bool CreateVolume(std::string volumeFileName, std::vector<std::string> filesToPack, std::vector<std::string> internalNames) = 0;

	protected:
		int OpenOutputFile(const char *fileName);
		void CloseOutputFile();
		bool ReplaceFileWithFile(const char *fileToReplace, const char *newFile);

		HANDLE m_OutFileHandle;
	};
}
