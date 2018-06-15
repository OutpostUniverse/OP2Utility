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
		if (MemoryMapFile(fileName)) {
			throw std::runtime_error("Could not open vol file " + std::string(fileName) + ".");
		}

		m_ArchiveFileSize = m_MappedFileSize;

		if (!ReadVolHeader()) {
			throw std::runtime_error("Invalid vol header in " + std::string(fileName) + ".");
		}
	}

	VolFile::~VolFile()
	{
	}



	std::string VolFile::GetInternalFileName(int index)
	{
		CheckPackedFileIndexBounds(index);

		return std::string(m_StringTable + m_Index[index].fileNameOffset);
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

		return m_Index[index].compressionType;
	}

	int VolFile::GetInternalFileSize(int index)
	{
		CheckPackedFileIndexBounds(index);

		return m_Index[index].fileSize;
	}



	int VolFile::GetInternalFileOffset(int index)
	{
		return m_Index[index].dataBlockOffset + 8;
	}

	int VolFile::GetInternalFileNameOffset(int index)
	{
		return m_Index[index].fileNameOffset;
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

		char* offset = (char*)m_BaseOfFile + m_Index[fileIndex].dataBlockOffset;
		std::size_t length = *(int*)(offset + 4) & 0x7FFFFFFF;
		offset += 8;

		return std::make_unique<MemoryStreamReader>(offset, length);
	}

	// Extracts the internal file at the given index to the fileName.
	void VolFile::ExtractFile(int fileIndex, const std::string& pathOut)
	{
		CheckPackedFileIndexBounds(fileIndex);

		if (m_Index[fileIndex].compressionType == CompressionType::Uncompressed)
		{
			ExtractFileUncompressed(fileIndex, pathOut);
		}
		else if (m_Index[fileIndex].compressionType == CompressionType::LZH)
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
			char* offset = (char*)m_BaseOfFile + m_Index[fileIndex].dataBlockOffset;
			int length = *(int*)(offset + 4) & 0x7FFFFFFF;
			offset += 8;
			
			FileStreamWriter fileStreamWriter(pathOut);
			fileStreamWriter.Write(offset, length);
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
			char* offset = (char*)m_BaseOfFile + m_Index[fileIndex].dataBlockOffset;
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

		// Close the currently opened file so it can be replaced
		UnmapFile();

		// Rename the output file to the desired file
		try {
			XFile::RenameFile(tempFileName, m_ArchiveFileName);
			return true;
		}
		catch (std::exception& e) {
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
		catch (std::exception& e) {
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

	void VolFile::CopyFileIntoVolume(StreamWriter& volWriter, HANDLE inputFile, int size)
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
	void VolFile::WriteTag(StreamWriter& volWriter, int length, const char *tagText)
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
	// invalid then this function returns -1.
	// Note: tagText is a 4 byte field
	int VolFile::ReadTag(int offset, const char *tagText)
	{
		char *addr = (char*)m_BaseOfFile + offset;

		if (*(int*)addr != *(int*)tagText) { 	   // Make sure the tag matches
			return -1;
		}

		int length = *(int*)(addr + 4);			   // Get the length (and length tag)
		if ((length & 0x80000000) != 0x80000000) { // Check for the tag (MSB set)
			return -1;
		}

		return length & 0x7FFFFFFF;				// Return the length (mask out the tag)
	}

	// Reads the header structure of the .vol file and sets up indexing/structure variables
	// Returns true is the header structure is valid and false otherwise
	bool VolFile::ReadVolHeader()
	{
		int temp;

		// Make sure file is big enough to contain header tag
		if (m_MappedFileSize < 8) {
			return false;
		}

		// Make sure the header is structured properly
		// -------------------------------------------
		// -------------------------------------------

		// Check opening header tag ("VOL " tag)
		// -------------------------------------
		if ((m_HeaderLength = ReadTag(0, "VOL ")) == -1) {
			return false;
		}

		// Make sure the file is large enough to contain the header
		if (m_MappedFileSize < m_HeaderLength + 8) {
			return false;
		}

		// Check next header tag ("volh" tag)
		// ----------------------------------
		// Note: the size of this section must be 0
		if (ReadTag(8, "volh") != 0) {
			return false;
		}

		// Check for fileName string table tag ("vols" tag)
		// ------------------------------------------------
		if ((m_StringTableLength = ReadTag(16, "vols")) == -1) {
			return false;
		}

		m_ActualStringTableLength = *((int*)m_BaseOfFile + 6);	// Store actual length
		m_StringTable = (char*)m_BaseOfFile + 28;				// Store pointer to table

		// Make sure the string table fits in the header (and next header fits too)
		if (m_HeaderLength < m_StringTableLength + 20) {
			return false;
		}

		temp = m_StringTableLength + 24;			// Calculate offset to next section
		// Check for volume index tag ("voli" tag)
		// ---------------------------------------
		// Store the offset to the Index table while it's still calculated
		m_Index = (IndexEntry*)((char*)m_BaseOfFile + temp + 8);
		if ((m_IndexTableLength = ReadTag(temp, "voli")) == -1) {
			return false;
		}

		// Make sure the index table fits in the header
		if (m_HeaderLength < m_StringTableLength + m_IndexTableLength + 24) {
			return false;
		}

		// Determine the number of index entries
		// -------------------------------------
		m_NumberOfIndexEntries = m_IndexTableLength / 14;
		// Count the number of valid entries
		for (temp = 0; temp < m_NumberOfIndexEntries; temp++)
		{
			// Make sure entry is valid
			if (m_Index[temp].fileNameOffset == UINT_MAX) {
				break;
			}
		}
		m_NumberOfPackedFiles = temp;		// Store the number of used index entries

		return true;						// Header read successfully
	}
}
