#include "ClmFile.h"
#include "../XFile.h"
#include <stdexcept>
#include <algorithm>

namespace Archives
{
	const uint32_t CLM_WRITE_SIZE = 0x00020000;

	ClmFile::ClmFile(const char *fileName) : ArchiveFile(fileName)
	{
		try {
			clmFileReader = std::make_unique<FileStreamReader>(fileName);
		}
		catch (std::exception& e) {
			throw std::runtime_error("Could not open clm file " + std::string(fileName) + ".");
		}

		try {
			ReadHeader();
		} 
		catch (std::exception& e) {
			throw std::runtime_error("Invalid clm header read from file " + std::string(fileName) + ". Internal error message: " + e.what() );
		}
	}

	ClmFile::~ClmFile() { }


	// Reads in the header when the volume is first opened and does some
	// basic error checking on the header.
	// Throws an error is problems encountered while reading the header.
	void ClmFile::ReadHeader()
	{
		clmFileReader->Read((char*)&clmHeader, sizeof(ClmHeader));
		
		if (!clmHeader.CheckFileVersion()) {
			throw std::runtime_error("CLM file version is incorrect.");
		}

		if (!clmHeader.CheckUnknown()) {
			throw std::runtime_error("Unknown field in CLM header is incorrect.");
		}

		if (clmHeader.packedFilesCount < 1) {
			throw std::runtime_error("Packed file count must be 1 or greater.");
		}

		m_NumberOfPackedFiles = clmHeader.packedFilesCount;

		indexEntries = std::vector<IndexEntry>(m_NumberOfPackedFiles);
		clmFileReader->Read((char*)indexEntries.data(), m_NumberOfPackedFiles * sizeof(IndexEntry));
		
		// Null terminate all packed fileNames.
		for (IndexEntry& indexEntry : indexEntries) {
			indexEntry.fileName[8] = 0;
		}
	}


	// Returns the internal file name of the packed file corresponding to index.
	// Throws an error if packed file index is not valid.
	std::string ClmFile::GetInternalFileName(int index)
	{
		CheckPackedFileIndexBounds(index);

		return indexEntries[index].GetFileName();
	}

	int ClmFile::GetInternalFileIndex(const char *internalFileName)
	{
		for (int i = 0; i < GetNumberOfPackedFiles(); ++i)
		{
			if (XFile::PathsAreEqual(GetInternalFileName(i), internalFileName)) {
				return i;
			}
		}

		return -1;
	}

	// Returns the size of the internal file corresponding to index
	int ClmFile::GetInternalFileSize(int index)
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

			waveFileWriter.Write((char*)&header, sizeof(header));

			// Seek to the beginning of the file data (in the .clm file)
			clmFileReader->Seek(indexEntries[fileIndex].dataOffset);

			uint32_t numBytesToRead = 0;
			uint32_t offset = 0; // Max size of CLM IndexEntry::dataLength is 32 bits.
			std::array<char, CLM_WRITE_SIZE> buffer;

			do
			{
				numBytesToRead = CLM_WRITE_SIZE;

				// Check if less than CLM_WRITE_SIZE of data remains for writing to disk.
				if (offset + numBytesToRead > indexEntries[fileIndex].dataLength) {
					numBytesToRead = indexEntries[fileIndex].dataLength - offset;
				}

				clmFileReader->Read(buffer.data(), CLM_WRITE_SIZE);

				offset += numBytesToRead;

				waveFileWriter.Write(buffer.data(), numBytesToRead);
			} while (numBytesToRead); // End loop when numBytesRead/Written is equal to 0
		}
		catch (std::exception& e)
		{
			throw std::runtime_error("Error attempting to extracted uncompressed file " + pathOut + ". Internal Error Message: " + e.what());
		}
	}

	void ClmFile::InitializeWaveHeader(WaveHeader& headerOut, int fileIndex)
	{
		headerOut.riffHeader.riffTag = RIFF;
		headerOut.riffHeader.waveTag = WAVE;
		headerOut.riffHeader.chunkSize = sizeof(headerOut.riffHeader.waveTag) + sizeof(FormatChunk) + sizeof(ChunkHeader) + indexEntries[fileIndex].dataLength;

		headerOut.formatChunk.fmtTag = FMT;
		headerOut.formatChunk.formatSize = sizeof(WaveFormatEx);
		headerOut.formatChunk.waveFormat = clmHeader.waveFormat;
		headerOut.formatChunk.waveFormat.cbSize = 0;

		headerOut.dataChunk.formatTag = DATA;
		headerOut.dataChunk.length = indexEntries[fileIndex].dataLength;
	}

	std::unique_ptr<SeekableStreamReader> ClmFile::OpenSeekableStreamReader(const char* internalFileName)
	{
		int fileIndex = GetInternalFileIndex(internalFileName);

		if (fileIndex < 0) {
			throw std::runtime_error("File does not exist in Archive.");
		}

		return OpenSeekableStreamReader(fileIndex);
	}

	std::unique_ptr<SeekableStreamReader> ClmFile::OpenSeekableStreamReader(int fileIndex)
	{
		CheckPackedFileIndexBounds(fileIndex);

		throw std::logic_error("OpenSeekableStreamReader not yet implemented for Clm files.");
	}

	// Repacks the volume using the same files as are specified by the internal file names
	// Returns nonzero if successful and zero otherwise
	bool ClmFile::Repack()
	{
		std::vector<std::string> filesToPack(m_NumberOfPackedFiles);
		std::vector<std::string> internalNames(m_NumberOfPackedFiles);

		for (int i = 0; i < m_NumberOfPackedFiles; i++)
		{
			//Filename is equivalent to internalName since filename is a relative path from current directory.
			filesToPack[i] = std::string(GetInternalFileName(i)) + ".wav";
		}

		return CreateArchive("temp.clm", filesToPack);
	}

	// Creates a new Archive file with the file name archiveFileName. The
	// files listed in the container filesToPack are packed into the archive.
	// Automatically strips file name extensions from filesToPack. 
	// Returns nonzero if successful and zero otherwise.
	bool ClmFile::CreateArchive(std::string archiveFileName, std::vector<std::string> filesToPack)
	{
		if (filesToPack.size() < 1) {
			return false; //CLM files require at least one audio file present to properly write settings.
		}

		// Sort files alphabetically based on the fileName only (not including the full path).
		// Packed files must be locatable by a binary search of their fileName.
		std::sort(filesToPack.begin(), filesToPack.end(), ComparePathFilenames);

		std::vector<std::unique_ptr<FileStreamReader>> filesToPackReaders;
		try {
			// Opens all files for packing. If there is a problem opening a file, an exception is raised.
			for (std::string& fileName : filesToPack) {
				filesToPackReaders.push_back(std::make_unique<FileStreamReader>(fileName));
			}
		}
		catch (std::exception& e) {
			return false;
		}

		// Initialize vectors with default values for the number of files to pack. 
		// Allows directly reading data into the vector using a StreamReader.
		std::vector<WaveFormatEx> waveFormats(filesToPack.size());
		std::vector<IndexEntry> indexEntries(filesToPack.size());

		if (!ReadAllWaveHeaders(filesToPackReaders, waveFormats, indexEntries)) {	
			return false; // Error reading in wave headers. Abort.
		}

		// Check if all wave formats are the same
		if (!CompareWaveFormats(waveFormats)) {	
			return false; // Not all wave formats match
		}

		std::vector<std::string> internalFileNames = GetInternalNamesFromPaths(filesToPack);
		internalFileNames = StripFileNameExtensions(internalFileNames);
		// Allowing duplicate names when packing may cause unintended results during binary search and file extraction.
		CheckSortedContainerForDuplicateNames(internalFileNames); 

		// Write the archive header and copy files into the archive
		try {
			WriteArchive(archiveFileName, filesToPackReaders, indexEntries, internalFileNames, waveFormats[0]);
		}
		catch (std::exception& e) {
			return false; // Error writing CLM archive file
		}

		return true;
	}


	// Reads the beginning of each file and verifies it is formatted as a WAVE file. Locates
	// the WaveFormatEx structure and start of data. The WaveFormat is stored in the waveFormats container.
	// The current stream position is set to the start of the data chunk.
	// Returns nonzero if successful and zero otherwise.
	// Note: This function assumes that all stream positions are initially set to the beginning
	//  of the file. When reading the wave file header, it does not seek to the file start.
	bool ClmFile::ReadAllWaveHeaders(std::vector<std::unique_ptr<FileStreamReader>>& filesToPackReaders, std::vector<WaveFormatEx>& waveFormats, std::vector<IndexEntry>& indexEntries)
	{
		RiffHeader header;

		// Read in all the headers and find start of data
		for (size_t i = 0; i < filesToPackReaders.size(); i++)
		{
			// Read the file header
			filesToPackReaders[i]->Read((char*)&header, sizeof(header));
			if (header.riffTag != RIFF || header.waveTag != WAVE) {
				return false;		// Error reading header
			}

			// Check that the file size makes sense (matches with header chunk length + 8)
			if (header.chunkSize + 8 != filesToPackReaders[i]->Length()) {
				return false;
			}

			// Read the format tag
			int fmtChunkLength = FindChunk(FMT, *filesToPackReaders[i]);
			if (fmtChunkLength == -1) {
				return false;		// Format chunk not found
			}

			// Read in the wave format
			filesToPackReaders[i]->Read((char*)&waveFormats[i], sizeof(WaveFormatEx));
			waveFormats[i].cbSize = 0;

			// Find the start of the data
			int dataChunkLength = FindChunk(DATA, *filesToPackReaders[i]);
			if (dataChunkLength == -1) {
				return false;		// Data chunk not found
			}

			// Store the length of the wave data
			indexEntries[i].dataLength = dataChunkLength;
			// Note: Current stream position is set to the start of the wave data
		}

		return true;
	}

	// Searches through the wave file to find the given chunk length
	// The current stream position is set the the first byte after the chunk header
	// Returns the chunk length if found or -1 otherwise
	int ClmFile::FindChunk(uint32_t chunkTag, SeekableStreamReader& seekableStreamReader)
	{
		uint64_t fileSize = seekableStreamReader.Length();

		if (fileSize < sizeof(RiffHeader) + sizeof(ChunkHeader)) {
			return -1;
		}

		// Seek to beginning of first internal chunk (provided it exists)
		// Note: this seeks past the initial format tag (such as RIFF and WAVE)
		uint32_t currentPosition = sizeof(RiffHeader);
		seekableStreamReader.Seek(currentPosition);

		ChunkHeader header;
		do
		{
			// Read the tag
			seekableStreamReader.Read((char*)&header, sizeof(header));
			
			// Check if this is the right header
			if (header.formatTag == chunkTag) {
				return header.length;
			}
			
			// If not the right header, skip to next header
			currentPosition += header.length + sizeof(ChunkHeader);
			seekableStreamReader.Seek(currentPosition);
		} while (currentPosition < fileSize);

		return -1;	// Failed to find the tag
	}

	// Compares wave format structures in the waveFormats container
	// Returns true if they are all the same and false otherwise.
	bool ClmFile::CompareWaveFormats(const std::vector<WaveFormatEx>& waveFormats)
	{
		for (size_t i = 1; i < waveFormats.size(); i++)
		{
			if (memcmp(&waveFormats[i], &waveFormats[0], sizeof(WaveFormatEx))) {
				return false;
			}
		}

		return true;
	}

	void ClmFile::WriteArchive(std::string& archiveFileName,
		std::vector<std::unique_ptr<FileStreamReader>>& filesToPackReaders,
		std::vector<IndexEntry>& indexEntries,
		const std::vector<std::string>& internalNames,
		const WaveFormatEx& waveFormat)
	{
		// ClmFile cannot contain more than 32 bit size internal file cound.
		ClmHeader header(waveFormat, static_cast<uint32_t>(internalNames.size()));

		FileStreamWriter clmFileWriter(archiveFileName);

		clmFileWriter.Write((char*)&header, sizeof(header));

		// Prepare and write Archive Index
		PrepareIndex(sizeof(header), internalNames, indexEntries);
		clmFileWriter.Write((char*)indexEntries.data(), header.packedFilesCount * sizeof(IndexEntry));

		// Copy files into the archive
		for (size_t i = 0; i < header.packedFilesCount; i++) {
			PackFile(clmFileWriter, indexEntries[i], *filesToPackReaders[i]);
		}
	}

	void ClmFile::PrepareIndex(int headerSize, const std::vector<std::string>& internalNames, std::vector<IndexEntry>& indexEntries)
	{
		uint32_t offset = headerSize + internalNames.size() * sizeof(IndexEntry);
		for (size_t i = 0; i < internalNames.size(); i++)
		{
			// Copy the filename into the entry
			strncpy((char*)&indexEntries[i].fileName, internalNames[i].c_str(), 8);

			// Set the offset of the file
			indexEntries[i].dataOffset = offset;
			offset += indexEntries[i].dataLength;
		}
	}

	// Write file into the Clm Archive by using the fixed memory size of CLM_WRITE_SIZE.
	void ClmFile::PackFile(StreamWriter& clmWriter, const IndexEntry& indexEntry, StreamReader& fileToPackReader)
	{
		uint32_t numBytesToRead;
		uint32_t offset = 0; // Max size of CLM IndexEntry::dataLength is 32 bits.
		std::array<char, CLM_WRITE_SIZE> buffer;

		do
		{
			numBytesToRead = CLM_WRITE_SIZE;

			// Check if less than CLM_WRITE_SIZE of data remains for writing to disk.
			if (offset + numBytesToRead > indexEntry.dataLength) {
				numBytesToRead = indexEntry.dataLength - offset;
			}

			// Read the input file
			fileToPackReader.Read(buffer.data(), numBytesToRead);
			offset += numBytesToRead;

			clmWriter.Write(buffer.data(), numBytesToRead);
		} while (numBytesToRead); // End loop when numBytesRead/Written is equal to 0
	}

	std::vector<std::string> ClmFile::StripFileNameExtensions(std::vector<std::string> paths)
	{
		std::vector<std::string> strippedExtensions;

		for (std::string& path : paths) {
			strippedExtensions.push_back(XFile::ChangeFileExtension(path, ""));
		}

		return strippedExtensions;
	}



	const std::array<char, 32> standardFileVersion { 'O', 'P', '2', ' ', 'C', 'l', 'u', 'm', 'p', ' ',
		'F', 'i', 'l', 'e', ' ', 'V', 'e', 'r', 's', 'i', 'o', 'n', ' ', '1', '.', '0', '\x01A', '\0', '\0', '\0', '\0' };
	const std::array<char, 6> standardUnknown { 0, 0, 0, 0, 1, 0 };

	ClmFile::ClmHeader::ClmHeader(WaveFormatEx waveFormat, uint32_t packedFilesCount) {
		std::memcpy(fileVersion.data(), standardFileVersion.data(), fileVersion.size());
		std::memcpy(unknown.data(), standardUnknown.data(), unknown.size());

		this->waveFormat = waveFormat;
		this->packedFilesCount = packedFilesCount;
	}

	bool ClmFile::ClmHeader::CheckFileVersion() const {
		return std::memcmp(fileVersion.data(), standardFileVersion.data(), fileVersion.size()) == 0;
	};

	bool ClmFile::ClmHeader::CheckUnknown() const {
		return std::memcmp(unknown.data(), standardUnknown.data(), unknown.size()) == 0;
	}
}
