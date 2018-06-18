#include "VolFile.h"
#include "../Streams/MemoryStreamReader.h"
#include "../Streams/FileStreamWriter.h"
#include "../XFile.h"
#include <stdexcept>
#include <algorithm>
#include <climits>

namespace Archives
{
	VolFile::VolFile(const char *fileName) : ArchiveFile(fileName)
	{
		archiveFileReader = std::make_unique<FileStreamReader>(fileName);

		// Archive file size is limited to unsigned 4 bytes in length
		m_ArchiveFileSize = static_cast<uint32_t>(archiveFileReader->Length());

		ReadVolHeader();
	}

	VolFile::~VolFile() { }



	std::string VolFile::GetInternalFileName(int index)
	{
		CheckPackedFileIndexBounds(index);

		return m_StringTable[index];
	}

	int VolFile::GetInternalFileIndex(const char *internalFileName)
	{
		for (int i = 0; i < GetNumberOfPackedFiles(); ++i)
		{
			std::string actualInternalFileName = GetInternalFileName(i);
			if (XFile::PathsAreEqual(actualInternalFileName, internalFileName)) {
				return i;
			}
		}

		return -1;
	}

	CompressionType VolFile::GetInternalCompressionCode(int index)
	{
		CheckPackedFileIndexBounds(index);

		return m_IndexEntries[index].compressionType;
	}

	int VolFile::GetInternalFileSize(int index)
	{
		CheckPackedFileIndexBounds(index);

		return m_IndexEntries[index].fileSize;
	}



	int VolFile::GetInternalFileOffset(int index)
	{
		return m_IndexEntries[index].dataBlockOffset + 8;
	}

	int VolFile::GetInternalFileNameOffset(int index)
	{
		return m_IndexEntries[index].fileNameOffset;
	}

	std::unique_ptr<SeekableStreamReader> VolFile::OpenSeekableStreamReader(const char* internalFileName)
	{
		int fileIndex = GetInternalFileIndex(internalFileName);

		if (fileIndex < 0) {
			throw std::runtime_error("File does not exist in Archive.");
		}

		return OpenSeekableStreamReader(fileIndex);
	}

	std::unique_ptr<SeekableStreamReader> VolFile::OpenSeekableStreamReader(int fileIndex)
	{
		CheckPackedFileIndexBounds(fileIndex);

		char* offset = (char*)m_IndexEntries[fileIndex].dataBlockOffset;
		std::size_t length = *(int*)(offset + 4) & 0x7FFFFFFF;
		offset += 8;

		return std::make_unique<MemoryStreamReader>(offset, length);
	}

	// Extracts the internal file at the given index to the fileName.
	void VolFile::ExtractFile(int fileIndex, const std::string& pathOut)
	{
		CheckPackedFileIndexBounds(fileIndex);

		if (m_IndexEntries[fileIndex].compressionType == CompressionType::Uncompressed)
		{
			ExtractFileUncompressed(fileIndex, pathOut);
		}
		else if (m_IndexEntries[fileIndex].compressionType == CompressionType::LZH)
		{
			ExtractFileLzh(fileIndex, pathOut);
		}
		else {
			throw std::runtime_error("Compression type is not supported."); 
		}
	}

	void VolFile::ExtractFileUncompressed(std::size_t fileIndex, const std::string& pathOut)
	{
		try
		{
			char* offset = (char*)m_IndexEntries[fileIndex].dataBlockOffset;
			std::size_t length = *(int*)(offset + 4) & 0x7FFFFFFF;
			offset += 8;
			
			FileStreamWriter fileStreamWriter(pathOut);
			fileStreamWriter.Write(&offset, length);
		}
		catch (std::exception e)
		{
			throw std::runtime_error("Error attempting to extracted uncompressed file " + pathOut + ". Internal Error Message: " + e.what());
		}
	}

	void VolFile::ExtractFileLzh(std::size_t fileIndex, const std::string& pathOut)
	{
		try
		{
			char* offset = (char*)m_IndexEntries[fileIndex].dataBlockOffset;
			int length = *(int*)(offset + 4) & 0x7FFFFFFF;
			offset += 8;

			HuffLZ decompressor(length, offset);
			const char *buffer = 0;

			FileStreamWriter fileStreamWriter(pathOut);

			do
			{
				buffer = decompressor.GetInternalBuffer(&length);
				fileStreamWriter.Write(buffer, length);
			} while (length);
		}
		catch (std::exception e)
		{
			throw std::runtime_error("Error attempting to extract LZH compressed file " + pathOut + ". Internal Error Message: " + e.what());
		}
	}



	bool VolFile::Repack()
	{
		std::vector<std::string> filesToPack(m_NumberOfPackedFiles);

		for (int i = 0; i < m_NumberOfPackedFiles; i++)
		{
			//Filename is equivalent to internalName since filename is a relative path from current directory.
			filesToPack.push_back(GetInternalFileName(i));
		}

		const std::string tempFileName("temp.vol");
		if (!CreateArchive(tempFileName, filesToPack)) {
			return false;
		}

		// Rename the output file to the desired file
		try {
			XFile::RenameFile(tempFileName, m_ArchiveFileName);
			return true;
		}
		catch (std::exception&) {
			return false;
		}
	}

	bool VolFile::CreateArchive(std::string volumeFileName, std::vector<std::string> filesToPack)
	{
		// Sort files alphabetically based on the fileName only (not including the full path).
		// Packed files must be locatable by a binary search of their fileName.
		std::sort(filesToPack.begin(), filesToPack.end(), ComparePathFilenames);

		CreateVolumeInfo volInfo;

		volInfo.filesToPack = filesToPack;
		volInfo.internalNames = GetInternalNamesFromPaths(filesToPack);

		// Allowing duplicate names when packing may cause unintended results during binary search and file extraction.
		CheckSortedContainerForDuplicateNames(volInfo.internalNames);

		// Open input files and prepare header and indexing info
		if (!PrepareHeader(volInfo)) {
			return false;
		}

		try {
			WriteVolume(volumeFileName, volInfo);
		}
		catch (std::exception&) {
			CleanUpVolumeCreate(volInfo);
			return false;
		}

		CleanUpVolumeCreate(volInfo);

		return true;
	}

	void VolFile::WriteVolume(const std::string& fileName, const CreateVolumeInfo& volInfo) 
	{
		FileStreamWriter volWriter(fileName);

		WriteHeader(volWriter, volInfo);
		WriteFiles(volWriter, volInfo);
	}

	void VolFile::CleanUpVolumeCreate(CreateVolumeInfo &volInfo)
	{
		// Close all input files
		for (auto& fileHandle : volInfo.fileHandles) {
			CloseHandle(fileHandle);
		}
	}

	void VolFile::WriteFiles(StreamWriter& volWriter, const CreateVolumeInfo &volInfo)
	{
		// Write each file header and contents
		for (std::size_t i = 0; i < volInfo.fileCount(); i++)
		{
			WriteTag(volWriter, volInfo.indexEntries[i].fileSize, "VBLK");

			try {
				CopyFileIntoVolume(volWriter, volInfo.fileHandles[i], volInfo.indexEntries[i].fileSize);
				int padding = 0;

				// Add padding after the file, ensuring it ends on a 4 byte boundary
				// Use a bitmask to quickly calculate the modulo 4 (remainder) of fileSize
				volWriter.Write(&padding, (-volInfo.indexEntries[i].fileSize) & 3);
			}
			catch (std::exception& e) {
				throw std::runtime_error("Unable to pack file " + volInfo.internalNames[i] + ". Internal error: " + e.what());
			}
		}
	}

	void VolFile::WriteHeader(StreamWriter& volWriter, const CreateVolumeInfo &volInfo)
	{
		// Write the header
		WriteTag(volWriter, volInfo.paddedStringTableLength + volInfo.paddedIndexTableLength + 24, "VOL ");

		WriteTag(volWriter, 0, "volh");

		// Write the string table
		WriteTag(volWriter, volInfo.paddedStringTableLength, "vols");

		volWriter.Write(&volInfo.stringTableLength, sizeof(volInfo.stringTableLength));

		// Write out all internal file name strings (including NULL terminator)
		for (std::size_t i = 0; i < volInfo.fileCount(); i++) {
			// Account for the null terminator in the size.
			volWriter.Write(volInfo.internalNames[i].c_str(), volInfo.internalNames[i].size() + 1);
		}

		int padding = 0; // Pad with 0 bytes
		volWriter.Write(&padding, volInfo.paddedStringTableLength - (volInfo.stringTableLength + 4));

		// Write the index table
		WriteTag(volWriter, volInfo.indexTableLength, "voli");

		volWriter.Write(volInfo.indexEntries.data(), volInfo.indexTableLength);

		volWriter.Write(&padding, volInfo.paddedIndexTableLength - volInfo.indexTableLength);
	}

	bool VolFile::OpenAllInputFiles(CreateVolumeInfo &volInfo)
	{
		volInfo.stringTableLength = 0;

		// Open all the input files and store indexing info
		for (std::size_t i = 0; i < volInfo.fileCount(); i++)
		{
			HANDLE fileHandle = CreateFileA(
				volInfo.filesToPack[i].c_str(),	// fileName
				GENERIC_READ,			// access mode
				0,						// share mode
				nullptr,				// security attributes
				OPEN_EXISTING,			// creation disposition
				FILE_ATTRIBUTE_NORMAL,	// file attributes
				nullptr);				// template
			
			volInfo.fileHandles.push_back(fileHandle);

			if (fileHandle == INVALID_HANDLE_VALUE)// Check for errors
			{
				// Error opening file. Close already opened files and return error
				for (i--; i >= 0; i--) {
					CloseHandle(volInfo.fileHandles[i]);
				}

				return false;
			}
		}

		return true;
	}

	bool VolFile::PrepareHeader(CreateVolumeInfo &volInfo)
	{
		if (!OpenAllInputFiles(volInfo)) {
			return false;
		}

		// Get file sizes and calculate length of string table
		for (std::size_t i = 0; i < volInfo.fileCount(); i++)
		{
			IndexEntry indexEntry;

			indexEntry.fileSize = GetFileSize(volInfo.fileHandles[i], 0);
			indexEntry.fileNameOffset = volInfo.stringTableLength;
			indexEntry.compressionType = CompressionType::Uncompressed;

			volInfo.indexEntries.push_back(indexEntry);

			// Add length of internal fileName plus null terminator to string table length.
			volInfo.stringTableLength += volInfo.internalNames[i].size() + 1;
		}

		// Calculate size of index table
		volInfo.indexTableLength = volInfo.fileCount() * sizeof(IndexEntry);
		// Calculate the zero padded length of the string table and index table
		volInfo.paddedStringTableLength = (volInfo.stringTableLength + 7) & ~3;
		volInfo.paddedIndexTableLength = (volInfo.indexTableLength + 3) & ~3;
		volInfo.indexEntries[0].dataBlockOffset = volInfo.paddedStringTableLength + volInfo.paddedIndexTableLength + 32;
		
		// Calculate offsets to the files
		for (std::size_t i = 1; i < volInfo.fileCount(); i++)
		{
			const IndexEntry& previousIndex = volInfo.indexEntries[i - 1];
			volInfo.indexEntries[i].dataBlockOffset = (previousIndex.dataBlockOffset + previousIndex.fileSize + 11) & ~3;
		}

		return true;
	}

	void VolFile::CopyFileIntoVolume(StreamWriter& volWriter, HANDLE inputFile, int32_t size)
	{
		char buffer[VOL_WRITE_SIZE];
		unsigned long numBytesRead;

		do
		{
			// Read a chunk from the input file
			if (ReadFile(inputFile, buffer, VOL_WRITE_SIZE, &numBytesRead, nullptr) == 0) {
				throw std::runtime_error("Error attempting to read from file for packing");
			}

			volWriter.Write(buffer, numBytesRead);

		} while (numBytesRead != 0);
	}

	// Writes a section tag to the open output file.
	void VolFile::WriteTag(StreamWriter& volWriter, uint32_t length, const char *tagText)
	{
		int buffer[2];

		buffer[0] = *(int*)tagText;
		// Set padding bit to indicate 4 byte padding
		buffer[1] = length | 0x80000000;

		try {
			volWriter.Write(buffer, sizeof(buffer));
		}
		catch (std::exception& e) {
			throw std::runtime_error("Unable to write tag " + std::string(tagText) + ". Internal Error: " + e.what());
		}
	}


	// Reads a tag in the .vol file and returns the length of that section.
	// If tagText does not match what is in the file or if the length is 
	// invalid then an error is thrown.
	uint32_t VolFile::ReadTag(const std::array<char, 4>& tagName)
	{
		// Tags are 4 chars in length and do not include a null terminator
		std::array<char, 4> tagFromFile;
		archiveFileReader->Read(tagFromFile.data(), tagFromFile.size());

		if (tagFromFile != tagName) {
			throw std::runtime_error("The tag " + std::string(tagName.data(), tagName.size()) + 
				" was not found in the proper position in volume " + m_ArchiveFileName);
		}

		uint32_t length = 0;
		archiveFileReader->Read(&length, sizeof(length));

		// Check for the tag (MSB set)
		if ((length & 0x80000000) != 0x80000000) {
			throw std::runtime_error("The tag " + std::string(tagName.data(), tagName.size()) + " from volume " + m_ArchiveFileName + " contains an invalid length.");
		}

		// Mask out the tag and return the length.
		return length & 0x7FFFFFFF;					
	}

	// Reads the header structure of the .vol file and sets up indexing/structure variables
	// Returns true is the header structure is valid and false otherwise
	void VolFile::ReadVolHeader()
	{
		m_HeaderLength = ReadTag(std::array<char, 4> { 'V', 'O', 'L', ' ' });

		uint32_t volhSize = ReadTag(std::array<char, 4> { 'v', 'o', 'l', 'h' });
		if (volhSize != 0) {
			throw std::runtime_error("The length associated with tag volh is not zero in volume " + m_ArchiveFileName);
		}

		m_StringTableLength = ReadTag(std::array<char, 4> { 'v', 'o', 'l', 's' });

		ReadStringTable();

		if (m_HeaderLength < m_StringTableLength + 20) {
			throw std::runtime_error("The string table does not fit in the header of volume " + m_ArchiveFileName);
		}

		m_IndexTableLength = ReadTag(std::array<char, 4> { 'v', 'o', 'l', 'i' });
		m_NumberOfIndexEntries = m_IndexTableLength / sizeof(IndexEntry);

		if (m_IndexTableLength > 0) {
			m_IndexEntries.resize(m_NumberOfIndexEntries);
			archiveFileReader->Read(m_IndexEntries.data(), m_IndexTableLength);
		}

		if (m_HeaderLength < m_StringTableLength + m_IndexTableLength + 24) {
			throw std::runtime_error("The index table does not fit in the header of volume " + m_ArchiveFileName);
		}

		ReadPackedFileCount();
	}

	void VolFile::ReadStringTable()
	{
		uint32_t actualStringTableLength;
		archiveFileReader->Read(&actualStringTableLength, sizeof(actualStringTableLength));

		std::string charBuffer;
		charBuffer.resize(actualStringTableLength);
		archiveFileReader->Read(&charBuffer[0], actualStringTableLength);

		m_StringTable.push_back("");
		for (std::size_t i = 0; i < charBuffer.size(); i++)
		{
			if (charBuffer[i] == '\0') {
				m_StringTable.push_back("");
				continue;
			}

			m_StringTable[m_StringTable.size() - 1].push_back(charBuffer[i]);
		}

		m_StringTable.erase(m_StringTable.begin() + m_StringTable.size() - 1);

		// Seek to the end of padding at end of StringTable
		archiveFileReader->SeekRelative(m_StringTableLength - actualStringTableLength - 4);
	}

	void VolFile::ReadPackedFileCount()
	{
		// Count the number of valid entries
		uint32_t packedFileCount = 0;
		for (packedFileCount; packedFileCount < m_NumberOfIndexEntries; packedFileCount++)
		{
			// Make sure entry is valid
			if (m_IndexEntries[packedFileCount].fileNameOffset == UINT_MAX) {
				break;
			}
		}
		m_NumberOfPackedFiles = packedFileCount;
	}
}
