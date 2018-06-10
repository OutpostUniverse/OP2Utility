#include "VolFile.h"
#include "../StreamReader.h"
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

		m_VolumeFileSize = m_MappedFileSize;

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

		if (!CreateArchive("Temp.vol", filesToPack)) {
			return false;
		}

		// Close the currently opened file so it can be replaced
		UnmapFile();

		// Rename the output file to the desired file
		return ReplaceFileWithFile(m_VolumeFileName.c_str(), "Temp.vol");
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
		for (size_t i = 0; i < volInfo.fileCount(); i++)
		{
			CloseHandle(volInfo.fileHandle[i]);
		}

		// Release memory used to create the index
		delete[] volInfo.indexEntry;
		delete[] volInfo.fileHandle;
		delete[] volInfo.fileNameLength;
	}

	void VolFile::WriteFiles(StreamWriter& volWriter, const CreateVolumeInfo &volInfo)
	{
		// Write each file header and contents
		for (size_t i = 0; i < volInfo.fileCount(); i++)
		{
			WriteTag(volWriter, volInfo.indexEntry[i].fileSize, "VBLK");

			try {
				CopyFileIntoVolume(volWriter, volInfo.fileHandle[i], volInfo.indexEntry[i].fileSize);
				int padding = 0;
				
				// Pad the end of the packed file to ensure the length is a multiple of 4 bytes
				// Use a bitmask to quickly calculate the modulo 4 (remainder) of fileSize
				volWriter.Write(&padding, (-volInfo.indexEntry[i].fileSize) & 3);
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
		for (size_t i = 0; i < volInfo.fileCount(); i++) {
			volWriter.Write(volInfo.internalNames[i].c_str(), volInfo.fileNameLength[i]);
		}

		int padding = 0; // Pad with 0 bytes
		volWriter.Write(&padding, volInfo.paddedStringTableLength - (volInfo.stringTableLength + 4));

		// Write the index table
		WriteTag(volWriter, volInfo.indexTableLength, "voli");

		volWriter.Write(volInfo.indexEntry, volInfo.indexTableLength);

		volWriter.Write(&padding, volInfo.paddedIndexTableLength - volInfo.indexTableLength);
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
		// Use a bitmask to increase tag size to a multiple of 4 bytes
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
			if (m_Index[temp].fileNameOffset == UINT_MAX)
				break;
		}
		m_NumberOfPackedFiles = temp;		// Store the number of used index entries

		return true;						// Header read successfully
	}
}
