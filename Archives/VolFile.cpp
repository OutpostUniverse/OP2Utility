#include "VolFile.h"
#include "../StreamReader.h"
#include "../StreamWriter.h"
#include "../XFile.h"
#include <stdexcept>
#include <algorithm>


namespace Archives
{
	VolFile::VolFile(const char *fileName) : ArchiveFile(fileName)
	{
		if (MemoryMapFile(fileName)) {
			throw std::runtime_error("Could not open vol file " + std::string(fileName) + ".");
		}

		m_VolumeFileSize = m_MappedFileSize;

		if (!ReadVolHeader()) {
			throw std::runtime_error("Invalid vol header in " + std::string(fileName) + ".");
		}
	}

	VolFile::~VolFile()
	{
	}



	const char* VolFile::GetInternalFileName(int index)
	{
		CheckPackedFileIndexBounds(index);

		return m_StringTable + m_Index[index].fileNameOffset;
	}

	int VolFile::GetInternalFileIndex(const char *internalFileName)
	{
		for (int i = 0; i < GetNumberOfPackedFiles(); ++i)
		{
			const char* actualInternalFileName = GetInternalFileName(i);
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
		size_t length = *(int*)(offset + 4) & 0x7FFFFFFF;
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

	void VolFile::ExtractFileUncompressed(size_t fileIndex, const std::string& pathOut)
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

	void VolFile::ExtractFileLzh(size_t fileIndex, const std::string& pathOut)
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
			throw std::runtime_error("Error attempting to extracted LZH compressed file " + pathOut + ". Internal Error Message: " + e.what());
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

		if (!CreateVolume("Temp.vol", filesToPack)) {
			return false;
		}

		// Close the currently opened file so it can be replaced
		UnmapFile();

		// Rename the output file to the desired file
		return ReplaceFileWithFile(m_VolumeFileName, "Temp.vol");
	}

	bool VolFile::CreateVolume(std::string volumeFileName, std::vector<std::string> filesToPack)
	{
		std::sort(filesToPack.begin(), filesToPack.end(), ComparePathFilenames);

		CreateVolumeInfo volInfo;

		volInfo.filesToPack = filesToPack;
		volInfo.internalNames = GetInternalNamesFromPaths(filesToPack);
		CheckSortedContainerForDuplicateNames(volInfo.internalNames);

		if (OpenOutputFile(volumeFileName.c_str()) == 0) {
			return false;
		}

		// Open input files and prepare header and indexing info
		if (!PrepareHeader(volInfo)) {
			CloseOutputFile();
			return false;
		}


		// Write volume contents
		// ---------------------
		// Write the header
		if (!WriteHeader(volInfo)) {
			CleanUpVolumeCreate(volInfo);
			return false;
		}
		
		if (!WriteFiles(volInfo)) {
			CleanUpVolumeCreate(volInfo);
			return false;
		}

		CleanUpVolumeCreate(volInfo);

		return true;
	}

	void VolFile::CleanUpVolumeCreate(CreateVolumeInfo &volInfo)
	{
		// Close all input files
		for (size_t i = 0; i < volInfo.fileCount(); i++)
		{
			CloseHandle(volInfo.fileHandle[i]);
		}

		CloseOutputFile();

		// Release memory used to create the index
		delete[] volInfo.indexEntry;
		delete[] volInfo.fileHandle;
		delete[] volInfo.fileNameLength;
	}

	bool VolFile::WriteFiles(CreateVolumeInfo &volInfo)
	{
		unsigned long numBytesWritten;
		int temp;

		// Write each file header and contents
		for (size_t i = 0; i < volInfo.fileCount(); i++)
		{
			if (WriteTag(volInfo.indexEntry[i].fileSize, "VBLK") == false) {
				return false;
			}

			if (CopyFileIntoVolume(volInfo.fileHandle[i], volInfo.indexEntry[i].fileSize) == false) {
				return false;
			}

			temp = 0; // Pad with 0 bytes

			if (WriteFile(m_OutFileHandle, &temp, (-volInfo.indexEntry[i].fileSize) & 3,
				&numBytesWritten, nullptr) == false) {
				return false;
			}
		}

		return true;
	}

	bool VolFile::WriteHeader(CreateVolumeInfo &volInfo)
	{
		unsigned long numBytesWritten;

		// Write the header
		if (WriteTag(volInfo.paddedStringTableLength + volInfo.paddedIndexTableLength + 24, "VOL ") == false) {
			return false;
		}

		if (WriteTag(0, "volh") == false) {
			return false;
		}

		// Write the string table
		if (WriteTag(volInfo.paddedStringTableLength, "vols") == false) {
			return false;
		}

		if (WriteFile(m_OutFileHandle, &volInfo.stringTableLength, 4, &numBytesWritten, nullptr) == 0) {
			return false;
		}

		// Write out all internal file name strings (including NULL terminator)
		for (size_t i = 0; i < volInfo.fileCount(); i++) {
			if (WriteFile(m_OutFileHandle, volInfo.internalNames[i].c_str(), volInfo.fileNameLength[i], &numBytesWritten, nullptr) == 0) return false;
		}

		int i = 0; // Pad with 0 bytes
		if (WriteFile(m_OutFileHandle, &i, volInfo.paddedStringTableLength - (volInfo.stringTableLength + 4), &numBytesWritten, nullptr) == 0) return false;

		// Write the index table
		if (WriteTag(volInfo.indexTableLength, "voli") == false) {
			return false;
		}

		if (WriteFile(m_OutFileHandle, volInfo.indexEntry, volInfo.indexTableLength, &numBytesWritten, nullptr) == 0) {
			return false;
		}

		if (WriteFile(m_OutFileHandle, &i, volInfo.paddedIndexTableLength - volInfo.indexTableLength, &numBytesWritten, nullptr) == 0) {
			return false;
		}

		return true;
	}

	bool VolFile::OpenAllInputFiles(CreateVolumeInfo &volInfo)
	{
		// Allocate space to keep track of all input files
		volInfo.indexEntry = new IndexEntry[volInfo.fileCount()];
		volInfo.fileHandle = new HANDLE[volInfo.fileCount()];
		volInfo.fileNameLength = new int[volInfo.fileCount()];
		volInfo.stringTableLength = 0;

		// Open all the input files and store indexing info
		for (size_t i = 0; i < volInfo.fileCount(); i++)
		{
			volInfo.fileHandle[i] = CreateFileA(
				volInfo.filesToPack[i].c_str(),	// fileName
				GENERIC_READ,			// access mode
				0,						// share mode
				nullptr,				// security attributes
				OPEN_EXISTING,			// creation disposition
				FILE_ATTRIBUTE_NORMAL,	// file attributes
				nullptr);				// template

			if (volInfo.fileHandle[i] == INVALID_HANDLE_VALUE)// Check for errors
			{
				// Error opening file. Close already opened files and return error
				for (i--; i >= 0; i--)
					CloseHandle(volInfo.fileHandle[i]);
				delete[] volInfo.indexEntry;
				delete[] volInfo.fileHandle;
				delete[] volInfo.fileNameLength;

				return false;
			}
		}

		return true;
	}

	bool VolFile::PrepareHeader(CreateVolumeInfo &volInfo)
	{
		IndexEntry *tempPtr;

		if (OpenAllInputFiles(volInfo) == false) return false;// Verify input files can be read

		// Get files sizes and calculate length of string table
		for (size_t i = 0; i < volInfo.fileCount(); i++)
		{
			// Store the indexing info into the index struct
			volInfo.indexEntry[i].fileSize = GetFileSize(volInfo.fileHandle[i], 0);
			volInfo.indexEntry[i].fileNameOffset = volInfo.stringTableLength;
			volInfo.indexEntry[i].compressionType = CompressionType::Uncompressed;
			// Calculate length of internal file names for string table
			volInfo.fileNameLength[i] = volInfo.internalNames[i].size() + 1; //Accounts for null terminator in size
			volInfo.stringTableLength += volInfo.fileNameLength[i];
		}

		// Calculate size of index table
		volInfo.indexTableLength = volInfo.fileCount() * sizeof(IndexEntry);
		// Calculate the zero padded length of the string table and index table
		volInfo.paddedStringTableLength = (volInfo.stringTableLength + 7) & ~3;
		volInfo.paddedIndexTableLength = (volInfo.indexTableLength + 3) & ~3;
		volInfo.indexEntry[0].dataBlockOffset = volInfo.paddedStringTableLength + volInfo.paddedIndexTableLength + 32;
		
		// Calculate offsets to the files
		for (size_t i = 1; i < volInfo.fileCount(); i++)
		{
			tempPtr = &volInfo.indexEntry[i - 1];
			volInfo.indexEntry[i].dataBlockOffset = (tempPtr->dataBlockOffset + tempPtr->fileSize + 11) & ~3;
		}

		return true;
	}

	bool VolFile::CopyFileIntoVolume(HANDLE inputFile, int size)
	{
		char buffer[VOL_WRITE_SIZE];
		unsigned long numBytesRead;
		unsigned long numBytesWritten;

		do
		{
			// Read a chunk from the input file
			if (ReadFile(inputFile, buffer, VOL_WRITE_SIZE, &numBytesRead, nullptr) == 0) return false;
			if (WriteFile(m_OutFileHandle, buffer, numBytesRead, &numBytesWritten, nullptr) == 0) return false;
		} while (numBytesRead != 0 && numBytesWritten != 0);

		return true;
	}

	// Writes a section tag to the open output file.
	bool VolFile::WriteTag(int length, const char *tagText)
	{
		unsigned long numBytesWritten;
		int buffer[2];

		buffer[0] = *(int*)tagText;
		buffer[1] = length | 0x80000000;

		return WriteFile(m_OutFileHandle, buffer, 8, &numBytesWritten, nullptr) != 0;
	}





	// Reads a tag in the .vol file and returns the length of that section.
	// If tagText does not match what is in the file or if the length is 
	// invalid then this function returns -1.
	// Note: tagText is a 4 byte field
	int VolFile::ReadTag(int offset, const char *tagText)
	{
		char *addr = (char*)m_BaseOfFile + offset;
		int length;

		if (*(int*)addr != *(int*)tagText)		// Make sure the tag matches
			return -1;

		length = *(int*)(addr + 4);				// Get the length (and length tag)
		if ((length & 0x80000000) != 0x80000000)// Check for the tag (MSB set)
			return -1;

		return length & 0x7FFFFFFF;				// Return the length (mask out the tag)
	}

	// Reads the header structure of the .vol file and sets up indexing/structure variables
	// Returns true is the header structure is valid and false otherwise
	bool VolFile::ReadVolHeader()
	{
		int temp;

		// Make sure file is big enough to contain header tag
		if (m_MappedFileSize < 8) return false;


		// Make sure the header is structured properly
		// -------------------------------------------
		// -------------------------------------------

		// Check opening header tag ("VOL " tag)
		// -------------------------------------
		if ((m_HeaderLength = ReadTag(0, "VOL ")) == -1) return false;
		// Make sure the file is large enough to contain the header
		if (m_MappedFileSize < m_HeaderLength + 8) return false;

		// Check next header tag ("volh" tag)
		// ----------------------------------
		// Note: the size of this section must be 0
		if (ReadTag(8, "volh") != 0) return false;

		// Check for fileName string table tag ("vols" tag)
		// ------------------------------------------------
		if ((m_StringTableLength = ReadTag(16, "vols")) == -1) return false;
		m_ActualStringTableLength = *((int*)m_BaseOfFile + 6);	// Store actual length
		m_StringTable = (char*)m_BaseOfFile + 28;				// Store pointer to table

		// Make sure the string table fits in the header (and next header fits too)
		if (m_HeaderLength < m_StringTableLength + 20)
			return false;

		temp = m_StringTableLength + 24;			// Calculate offset to next section
		// Check for volume index tag ("voli" tag)
		// ---------------------------------------
		// Store the offset to the Index table while it's still calculated
		m_Index = (IndexEntry*)((char*)m_BaseOfFile + temp + 8);
		if ((m_IndexTableLength = ReadTag(temp, "voli")) == -1) return false;
		// Make sure the index table fits in the header
		if (m_HeaderLength < m_StringTableLength + m_IndexTableLength + 24) return false;

		// Determine the number of index entries
		// -------------------------------------
		m_NumberOfIndexEntries = m_IndexTableLength / 14;
		// Count the number of valid entries
		for (temp = 0; temp < m_NumberOfIndexEntries; temp++)
		{
			// Make sure entry is valid
			if (m_Index[temp].fileNameOffset == -1)
				break;
		}
		m_NumberOfPackedFiles = temp;		// Store the number of used index entries

		return true;						// Header read successfully
	}
}
