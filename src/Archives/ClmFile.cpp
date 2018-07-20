#include "ClmFile.h"
#include "../Streams/FileSliceReader.h"
#include "../XFile.h"
#include <stdexcept>
#include <algorithm>

namespace Archives
{
	ClmFile::ClmFile(const std::string& filename) : ArchiveFile(filename), clmFileReader(filename)
	{
		m_ArchiveFileSize = clmFileReader.Length();
		ReadHeader();
	}

	ClmFile::~ClmFile() { }


	// Reads in the header when the volume is first opened and does some
	// basic error checking on the header.
	// Throws an error is problems encountered while reading the header.
	void ClmFile::ReadHeader()
	{
		clmFileReader.Read(clmHeader);

		try {
			clmHeader.VerifyFileVersion();
			clmHeader.VerifyUnknown();
		}
		catch (const std::exception& e) {
			throw std::runtime_error("Invalid clm header read from file " + m_ArchiveFilename + ". " + e.what());
		}

		m_NumberOfPackedFiles = clmHeader.packedFilesCount;

		indexEntries = std::vector<IndexEntry>(m_NumberOfPackedFiles);
		clmFileReader.Read(indexEntries.data(), m_NumberOfPackedFiles * sizeof(IndexEntry));
	}


	// Returns the internal file name of the packed file corresponding to index.
	// Throws an error if packed file index is not valid.
	std::string ClmFile::GetInternalFilename(int index)
	{
		CheckPackedFileIndexBounds(index);

		return indexEntries[index].GetFilename();
	}

	int ClmFile::GetInternalFileIndex(const std::string& internalFilename)
	{
		for (int i = 0; i < GetNumberOfPackedFiles(); ++i)
		{
			if (XFile::PathsAreEqual(GetInternalFilename(i), internalFilename)) {
				return i;
			}
		}

		return -1;
	}

	// Returns the size of the internal file corresponding to index
	uint32_t ClmFile::GetInternalFileSize(int index)
	{
		CheckPackedFileIndexBounds(index);

		return indexEntries[index].dataLength;
	}


	// Extracts the internal file corresponding to index
	void ClmFile::ExtractFile(int fileIndex, const std::string& pathOut)
	{
		CheckPackedFileIndexBounds(fileIndex);

		WaveHeader header;
		InitializeWaveHeader(header, fileIndex);

		try
		{
			FileStreamWriter waveFileWriter(pathOut);

			waveFileWriter.Write(header);

			FileSliceReader reader = clmFileReader.Slice(
				indexEntries[fileIndex].dataOffset,
				indexEntries[fileIndex].dataLength);

			ArchiveUnpacker::WriteFromStream(waveFileWriter, reader, reader.Length());
		}
		catch (const std::exception& e)
		{
			throw std::runtime_error("Error attempting to extracted uncompressed file " + pathOut + ". Internal Error Message: " + e.what());
		}
	}

	void ClmFile::InitializeWaveHeader(WaveHeader& headerOut, int fileIndex)
	{
		headerOut.riffHeader.riffTag = tagRIFF;
		headerOut.riffHeader.waveTag = tagWAVE;
		headerOut.riffHeader.chunkSize = sizeof(headerOut.riffHeader.waveTag) + sizeof(FormatChunk) + sizeof(ChunkHeader) + indexEntries[fileIndex].dataLength;

		headerOut.formatChunk.fmtTag = tagFMT_;
		headerOut.formatChunk.formatSize = sizeof(headerOut.formatChunk.waveFormat);
		headerOut.formatChunk.waveFormat = clmHeader.waveFormat;
		headerOut.formatChunk.waveFormat.cbSize = 0;

		headerOut.dataChunk.formatTag = tagDATA;
		headerOut.dataChunk.length = indexEntries[fileIndex].dataLength;
	}

	std::unique_ptr<SeekableStreamReader> ClmFile::OpenStream(int fileIndex)
	{
		CheckPackedFileIndexBounds(fileIndex);

		throw std::logic_error("OpenStream not yet implemented for Clm files.");
	}

	// Repacks the volume using the same files as are specified by the internal file names
	// Returns nonzero if successful and zero otherwise
	void ClmFile::Repack()
	{
		std::vector<std::string> filesToPack(m_NumberOfPackedFiles);
		std::vector<std::string> internalNames(m_NumberOfPackedFiles);

		for (int i = 0; i < m_NumberOfPackedFiles; ++i)
		{
			//Filename is equivalent to internalName since filename is a relative path from current directory.
			filesToPack[i] = GetInternalFilename(i) + ".wav";
		}

		const std::string tempFilename = "temp.clm";
		CreateArchive(tempFilename, filesToPack);

		// Rename the output file to the desired file
		XFile::RenameFile(tempFilename, m_ArchiveFilename);
	}

	// Creates a new Archive file with the file name archiveFilename. The
	// files listed in the container filesToPack are packed into the archive.
	// Automatically strips file name extensions from filesToPack.
	// Returns nonzero if successful and zero otherwise.
	void ClmFile::CreateArchive(const std::string& archiveFilename, std::vector<std::string> filesToPack)
	{
		// Sort files alphabetically based on the filename only (not including the full path).
		// Packed files must be locatable by a binary search of their filename.
		std::sort(filesToPack.begin(), filesToPack.end(), ComparePathFilenames);

		std::vector<std::unique_ptr<FileStreamReader>> filesToPackReaders;

		// Opens all files for packing. If there is a problem opening a file, an exception is raised.
		for (const auto& filename : filesToPack) {
			filesToPackReaders.push_back(std::make_unique<FileStreamReader>(filename));
		}

		// Initialize vectors with default values for the number of files to pack.
		// Allows directly reading data into the vector using a StreamReader.
		std::vector<WaveFormatEx> waveFormats(filesToPack.size());
		std::vector<IndexEntry> indexEntries(filesToPack.size());

		ReadAllWaveHeaders(filesToPackReaders, waveFormats, indexEntries);

		// Check if all wave formats are the same
		CompareWaveFormats(waveFormats, filesToPack);

		std::vector<std::string> internalFilenames = GetInternalNamesFromPaths(filesToPack);
		internalFilenames = StripFilenameExtensions(internalFilenames);
		// Allowing duplicate names when packing may cause unintended results during binary search and file extraction.
		CheckSortedContainerForDuplicateNames(internalFilenames);

		// Write the archive header and copy files into the archive
		WriteArchive(archiveFilename, filesToPackReaders, indexEntries, internalFilenames, PrepareWaveFormat(waveFormats));
	}

	// Reads the beginning of each file and verifies it is formatted as a WAVE file. Locates
	// the WaveFormatEx structure and start of data. The WaveFormat is stored in the waveFormats container.
	// The current stream position is set to the start of the data chunk.
	// Note: This function assumes that all stream positions are initially set to the beginning
	//  of the file. When reading the wave file header, it does not seek to the file start.
	void ClmFile::ReadAllWaveHeaders(std::vector<std::unique_ptr<FileStreamReader>>& filesToPackReaders, std::vector<WaveFormatEx>& waveFormats, std::vector<IndexEntry>& indexEntries)
	{
		RiffHeader header;

		// Read in all the headers and find start of data
		for (std::size_t i = 0; i < filesToPackReaders.size(); ++i)
		{
			// Read the file header
			filesToPackReaders[i]->Read(header);
			if (header.riffTag != tagRIFF || header.waveTag != tagWAVE) {
				throw std::runtime_error("Error reading header from file " + indexEntries[i].GetFilename());
			}

			// Check that the file size makes sense (matches with header chunk length + 8)
			if (header.chunkSize + 8 != filesToPackReaders[i]->Length()) {
				throw std::runtime_error("Chunk size does not match file length in " + indexEntries[i].GetFilename());
			}

			// Find the format tag
			FindChunk(tagFMT_, *filesToPackReaders[i]);
			// Read in the wave format
			filesToPackReaders[i]->Read(waveFormats[i]);
			waveFormats[i].cbSize = 0;

			// Find the start of the data and record length
			indexEntries[i].dataLength = FindChunk(tagDATA, *filesToPackReaders[i]);
			// Note: Current stream position is set to the start of the wave data
		}
	}

	// Searches through the wave file to find the given chunk length
	// The current stream position is set the the first byte after the chunk header
	// Returns the chunk length if found or -1 otherwise
	uint32_t ClmFile::FindChunk(std::array<char, 4> chunkTag, SeekableStreamReader& seekableStreamReader)
	{
		uint64_t fileSize = seekableStreamReader.Length();

		if (fileSize < sizeof(RiffHeader) + sizeof(ChunkHeader)) {
			throw std::runtime_error("There is not enough space in the file to represent the Riff Header and Chunk Header");
		}

		// Seek to beginning of first internal chunk (provided it exists)
		// Note: this seeks past the initial format tag (such as RIFF and WAVE)
		uint32_t currentPosition = sizeof(RiffHeader);
		seekableStreamReader.Seek(currentPosition);

		ChunkHeader header;
		do
		{
			seekableStreamReader.Read(header);

			// Check if this is the right header
			if (header.formatTag == chunkTag) {
				return header.length;
			}

			// If not the right header, skip to next header
			currentPosition += header.length + sizeof(ChunkHeader);
			seekableStreamReader.Seek(currentPosition);
		} while (currentPosition < fileSize);

		throw std::runtime_error("Unable to find the tag " + std::string(chunkTag.data(), chunkTag.size()));
	}

	// Compares wave format structures in the waveFormats container
	// If 2 wave formats are discovered of different type, an error is thrown.
	void ClmFile::CompareWaveFormats(const std::vector<WaveFormatEx>& waveFormats, const std::vector<std::string>& filesToPack)
	{
		for (std::size_t i = 0; i < waveFormats.size(); ++i)
		{
			if (memcmp(&waveFormats[0], &waveFormats[i], sizeof(WaveFormatEx))) {
				throw std::runtime_error("Files " + filesToPack[0] + " and " + filesToPack[i] +
					" contain differnt wave formats. Clm files cannot contain 2 wave files with different formats.");
			}
		}
	}

	void ClmFile::WriteArchive(const std::string& archiveFilename,
		const std::vector<std::unique_ptr<FileStreamReader>>& filesToPackReaders,
		std::vector<IndexEntry>& indexEntries,
		const std::vector<std::string>& internalNames,
		const WaveFormatEx& waveFormat)
	{
		// ClmFile cannot contain more than 32 bit size internal file count.
		ClmHeader header(waveFormat, static_cast<uint32_t>(internalNames.size()));

		FileStreamWriter clmFileWriter(archiveFilename);

		clmFileWriter.Write(header);

		// Prepare and write Archive Index
		PrepareIndex(sizeof(header), internalNames, indexEntries);
		clmFileWriter.Write(indexEntries.data(), header.packedFilesCount * sizeof(IndexEntry));

		// Copy files into the archive
		for (std::size_t i = 0; i < header.packedFilesCount; ++i) {
			ArchivePacker::WriteFromStream(clmFileWriter, *filesToPackReaders[i], indexEntries[i].dataLength);
		}
	}

	void ClmFile::PrepareIndex(int headerSize, const std::vector<std::string>& internalNames, std::vector<IndexEntry>& indexEntries)
	{
		uint32_t offset = headerSize + internalNames.size() * sizeof(IndexEntry);
		for (std::size_t i = 0; i < internalNames.size(); ++i)
		{
			// Copy the filename into the entry
			std::strncpy(indexEntries[i].filename.data(), internalNames[i].data(), sizeof(IndexEntry::filename));

			// Set the offset of the file
			indexEntries[i].dataOffset = offset;
			offset += indexEntries[i].dataLength;
		}
	}

	std::vector<std::string> ClmFile::StripFilenameExtensions(std::vector<std::string> paths)
	{
		std::vector<std::string> strippedExtensions;

		for (const auto& path : paths) {
			strippedExtensions.push_back(XFile::ChangeFileExtension(path, ""));
		}

		return strippedExtensions;
	}

	WaveFormatEx ClmFile::PrepareWaveFormat(const std::vector<WaveFormatEx>& waveFormats)
	{
		if (waveFormats.empty()) {
			return WaveFormatEx{
				1, // WAVE_FORMAT_PCM
				1, // mono
				22050, // 22.05KHz
				44100, // nSamplesPerSec * nBlockAlign
				2, // 2 bytes/sample = nChannels * wBitsPerSample / 8
				16,
				0
			};
		}

		return waveFormats.front();
	}


	// A null terminator (\0) is automatically assigned to the end of the string when placing it within the std::array
	const std::array<char, 32> standardFileVersion { "OP2 Clump File Version 1.0\x01A\0\0\0\0" };
	const std::array<char, 6> standardUnknown { 0, 0, 0, 0, 1, 0 };

	ClmFile::ClmHeader::ClmHeader(WaveFormatEx waveFormat, uint32_t packedFilesCount) {
		fileVersion = standardFileVersion;
		unknown = standardUnknown;

		this->waveFormat = waveFormat;
		this->packedFilesCount = packedFilesCount;
	}

	bool ClmFile::ClmHeader::CheckFileVersion() const {
		return fileVersion == standardFileVersion;
	};

	bool ClmFile::ClmHeader::CheckUnknown() const {
		return unknown == standardUnknown;
	}

	void ClmFile::ClmHeader::VerifyFileVersion() const {
		if (!CheckFileVersion()) {
			throw std::runtime_error("CLM file version is incorrect.");
		}
	}

	void ClmFile::ClmHeader::VerifyUnknown() const {
		if (!CheckUnknown()) {
			throw std::runtime_error("Unknown field in CLM header is incorrect.");
		}
	}

	std::string ClmFile::IndexEntry::GetFilename() const {
		// Find the first instance of the null terminator and return only this portion of the filename.
		auto firstNullTerminator = std::find(filename.begin(), filename.end(), '\0');

		return std::string(filename.data(), firstNullTerminator - filename.begin());
	}
}
