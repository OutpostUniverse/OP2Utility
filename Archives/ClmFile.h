#pragma once

#include "ArchiveFile.h"
#include "../StreamReader.h"
#include "../StreamWriter.h"
#include <windows.h>
#include <cstdint>
#include <string>
#include <vector>
#include <memory>

namespace Archives
{
	class ClmFile : public ArchiveFile
	{
	public:
		ClmFile(const char *fileName);
		virtual ~ClmFile();

		const char* GetInternalFileName(int index);
		int GetInternalFileIndex(const char *internalFileName);
		void ExtractFile(int fileIndex, const std::string& pathOut);
		std::unique_ptr<SeekableStreamReader> OpenSeekableStreamReader(const char *internalFileName);
		std::unique_ptr<SeekableStreamReader> OpenSeekableStreamReader(int fileIndex);

		int GetInternalFileSize(int index);

		bool Repack();
		bool CreateArchive(std::string archiveFileName, std::vector<std::string> filesToPack);

	private:
#pragma pack(push, 1)
		/*
		*  extended waveform format structure used for all non-PCM formats. this
		*  structure is common to all non-PCM formats.
		*  Identical to Windows.h WAVEFORMATEX typedef contained in mmeapi.h
		*/
		struct WaveFormatEx
		{
			uint16_t wFormatTag;         /* format type */
			uint16_t nChannels;          /* number of channels (i.e. mono, stereo...) */
			uint32_t nSamplesPerSec;     /* sample rate */
			uint32_t nAvgBytesPerSec;    /* for buffer estimation */
			uint16_t nBlockAlign;        /* block size of data */
			uint16_t wBitsPerSample;     /* number of bits per sample of mono data */
			uint16_t cbSize;             /* the count in bytes of the size of extra information (after cbSize) */
		};

		struct RiffHeader
		{
			int32_t riffTag;
			uint32_t chunkSize;
			int32_t waveTag;
		};

		struct FormatChunk
		{
			int fmtTag;
			int formatSize;
			WaveFormatEx waveFormat;
		};

		struct DataChunk
		{
			int dataTag;
			int dataSize;
		};

		struct WaveHeader
		{
			RiffHeader riffHeader;
			FormatChunk formatChunk;
			DataChunk dataChunk;
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
		bool ReadHeader();

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

		HANDLE m_FileHandle;
		WaveFormatEx m_WaveFormat;
		char m_Unknown[6];
		IndexEntry *m_IndexEntry;
		char(*m_FileName)[9];
	};
}
