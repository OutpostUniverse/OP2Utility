#pragma once

#include "WaveFile.h"
#include "ArchiveFile.h"
#include "../StreamReader.h"
#include "../StreamWriter.h"
#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <memory>

namespace Archives
{
	class ClmFile : public ArchiveFile
	{
	public:
		ClmFile(const char *fileName);
		virtual ~ClmFile();

		std::string GetInternalFileName(int index);
		int GetInternalFileIndex(const char *internalFileName);
		void ExtractFile(int fileIndex, const std::string& pathOut);
		std::unique_ptr<SeekableStreamReader> OpenSeekableStreamReader(const char *internalFileName);
		std::unique_ptr<SeekableStreamReader> OpenSeekableStreamReader(int fileIndex);

		int GetInternalFileSize(int index);

		bool Repack();
		bool CreateArchive(std::string archiveFileName, std::vector<std::string> filesToPack);

	private:
#pragma pack(push, 1)
		struct ClmHeader
		{
			ClmHeader() {};
			ClmHeader(WaveFormatEx waveFormatEx, uint32_t packFileCount);

			std::array<char, 32> fileVersion;
			WaveFormatEx waveFormat;
			std::array<char, 6> unknown;
			uint32_t packedFilesCount;

			bool CheckFileVersion() const;
			bool CheckUnknown() const;
		};

		struct IndexEntry
		{
			char fileName[8];
			unsigned int dataOffset;
			unsigned int dataLength;

			std::string GetFileName() const { return std::string(fileName); }
		};
#pragma pack(pop)

		// Private functions for reading archive
		void ReadHeader();

		// Private functions for packing files
		bool ReadAllWaveHeaders(std::vector<std::unique_ptr<FileStreamReader>>& filesToPackReaders, std::vector<WaveFormatEx>& waveFormats, std::vector<IndexEntry>& indexEntries);
		int FindChunk(uint32_t chunkTag, SeekableStreamReader& seekableStreamReader);
		bool CompareWaveFormats(const std::vector<WaveFormatEx>& waveFormats);
		void WriteArchive(std::string& archiveFileName, std::vector<std::unique_ptr<FileStreamReader>>& filesToPackReaders,
			std::vector<IndexEntry>& indexEntries, const std::vector<std::string>& internalNames, const WaveFormatEx& waveFormat);
		void PrepareIndex(int headerSize, const std::vector<std::string>& internalNames, std::vector<IndexEntry>& indexEntries);
		void PackFile(StreamWriter& clmWriter, const IndexEntry& entry, StreamReader& fileToPackReader);
		std::vector<std::string> StripFileNameExtensions(std::vector<std::string> paths);
		void InitializeWaveHeader(WaveHeader& headerOut, int fileIndex);

		std::unique_ptr<SeekableStreamReader> clmFileReader;
		ClmHeader clmHeader;
		std::vector<IndexEntry> indexEntries;
	};
}
