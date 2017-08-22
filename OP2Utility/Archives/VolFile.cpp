#include "VolFile.h"
#include "../XFile.h"

namespace Archives
{
	VolFile::VolFile(const char *filename) : ArchiveFile(filename)
	{
		// Memory map the .vol file
		if (MemoryMapFile(filename))
			throw "Could not open file";		// Error opening file

		m_VolumeFileSize = m_MappedFileSize;	// Store the .vol file size

		// Read in the header
		if (!ReadVolHeader())
			throw "Invalid header";				// Error reading in .vol header
	}

	VolFile::~VolFile()
	{
	}



	const char* VolFile::GetInternalFileName(int index)
	{
		return m_StringTable + m_Index[index].fileNameOffset;
	}

	int VolFile::GetInternalFileIndex(const char *internalFileName)
	{
		for (int i = 0; i < GetNumberOfPackedFiles(); ++i)
		{
			const char* actualInternalFileName = GetInternalFileName(i);
			if (XFile::pathsAreEqual(actualInternalFileName, internalFileName))
				return i;
		}

		return -1;
	}

	CompressionType VolFile::GetInternalCompressionCode(int index)
	{
		return m_Index[index].compressionType;
	}

	int VolFile::GetInternalFileSize(int index)
	{
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

	SeekableStreamReader* VolFile::OpenSeekableStreamReader(const char* internalFileName)
	{
		int fileIndex = GetInternalFileIndex(internalFileName);

		if (fileIndex < 0)
			throw exception("File does not exist in Archive.");

		return OpenSeekableStreamReader(fileIndex);
	}

	SeekableStreamReader* VolFile::OpenSeekableStreamReader(int fileIndex)
	{
		char* offset = (char*)m_BaseOfFile + m_Index[fileIndex].dataBlockOffset;
		size_t length = *(int*)(offset + 4) & 0x7FFFFFFF;
		offset += 8;

		return new MemoryStreamReader(offset, length);
	}

	// Extracts the internal file at the given index to the file 
	// filename. Returns nonzero if successful.
	int VolFile::ExtractFile(int index, const char *filename)
	{
		HANDLE outFile;
		unsigned long bytesWritten;
		int retVal;

		outFile = CreateFileA(filename,					// filename
			GENERIC_WRITE,			// access mode
			0,						// share mode
			NULL,					// security attributes
			CREATE_ALWAYS,			// creation disposition
			FILE_ATTRIBUTE_NORMAL,	// file attributes
			0);						// template

		// Check for errors opening file
		if (m_FileHandle == INVALID_HANDLE_VALUE)
			return false; // Error opening file

		char* offset = (char*)m_BaseOfFile + m_Index[index].dataBlockOffset;
		int length = *(int*)(offset + 4) & 0x7FFFFFFF;
		offset += 8;

		if (m_Index[index].compressionType == CompressionType::Uncompressed)
		{
			retVal = WriteFile(outFile, offset, length, &bytesWritten, NULL);
		}
		else
		{
			if (m_Index[index].compressionType == CompressionType::LZH)
			{
				// Decompress the file
				// Construct the decompressor
				HuffLZ decomp(length, offset);
				const char *buff = 0;

				do
				{
					buff = decomp.GetInternalBuffer(&length);
					retVal = WriteFile(outFile, buff, length, &bytesWritten, NULL);
				} while (length);
			}
			else
				retVal = 0;
		}

		// Close the file
		CloseHandle(outFile);

		return retVal;
	}



	bool VolFile::Repack()
	{
		int i;
		bool bRet;
		const char **filesToPack = new const char*[m_NumberOfPackedFiles];
		const char **internalNames = new const char*[m_NumberOfPackedFiles];

		// Create a list of filenames
		for (i = 0; i < m_NumberOfPackedFiles; i++)
		{
			// Get the internal filename
			internalNames[i] = GetInternalFileName(i);
			// Generate the external filename
			filesToPack[i] = internalNames[i];
		}

		// Repack the .vol file
		bRet = CreateVolume("Temp.vol", m_NumberOfPackedFiles, filesToPack, internalNames);

		delete[] internalNames;
		delete[] filesToPack;

		if (bRet == false) return false;

		// Close the currently opened file so it can be replaced
		UnmapFile();
		// Rename the output file to the desired file
		return ReplaceFileWithFile(m_VolumeFileName, "Temp.vol");
	}

	bool VolFile::CreateVolume(const char *volumeName, int numFilesToPack,
		const char **filesToPack, const char **internalNames)
	{
		CreateVolumeInfo volInfo;

		volInfo.numFilesToPack = numFilesToPack;
		volInfo.filesToPack = filesToPack;
		volInfo.internalNames = internalNames;

		// Make sure files are specified properly.
		if (numFilesToPack > 0)
			if (filesToPack == NULL || internalNames == NULL)
				return false;
		
		// Open a file for output
		if (OpenOutputFile(volumeName) == 0) 
			return false;	// Return false on error

		// Open input files and prepare header and indexing info
		if (!PrepareHeader(volInfo))
		{
			CloseOutputFile();
			return false;
		}


		// Write volume contents
		// ---------------------
		// Write the header
		if (!WriteHeader(volInfo))
		{
			CleanUpVolumeCreate(volInfo);
			return false;
		}
		
		if (!WriteFiles(volInfo))
		{
			CleanUpVolumeCreate(volInfo);
			return false;
		}

		CleanUpVolumeCreate(volInfo);

		return true;
	}

	void VolFile::CleanUpVolumeCreate(CreateVolumeInfo &volInfo)
	{
		int i;

		// Close all input files
		for (i = 0; i < volInfo.numFilesToPack; i++)
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
		int i;
		int temp;

		// Write each file header and contents
		for (i = 0; i < volInfo.numFilesToPack; i++)
		{
			// Write out the block tag
			if (WriteTag(volInfo.indexEntry[i].fileSize, "VBLK") == false) return false;
			// Copy the file into the volume
			if (CopyFileIntoVolume(volInfo.fileHandle[i], volInfo.indexEntry[i].fileSize) == false) return false;
			temp = 0;			// Pad with 0 bytes
			if (WriteFile(m_OutFileHandle, &temp, (-volInfo.indexEntry[i].fileSize) & 3,
				&numBytesWritten, NULL) == false) return false;
		}

		return true;
	}

	bool VolFile::WriteHeader(CreateVolumeInfo &volInfo)
	{
		unsigned long numBytesWritten;
		int i;

		// Write the header
		if (WriteTag(volInfo.paddedStringTableLength + volInfo.paddedIndexTableLength + 24, "VOL ") == false) return false;
		if (WriteTag(0, "volh") == false) return false;

		// Write the string table
		if (WriteTag(volInfo.paddedStringTableLength, "vols") == false) return false;
		if (WriteFile(m_OutFileHandle, &volInfo.stringTableLength, 4, &numBytesWritten, NULL) == 0) return false;
		// Write out all internal file name strings (including NULL terminator)
		for (i = 0; i < volInfo.numFilesToPack; i++)
			if (WriteFile(m_OutFileHandle, volInfo.internalNames[i], volInfo.fileNameLength[i], &numBytesWritten, NULL) == 0) return false;
		i = 0;			// Pad with 0 bytes
		if (WriteFile(m_OutFileHandle, &i, volInfo.paddedStringTableLength - (volInfo.stringTableLength + 4), &numBytesWritten, NULL) == 0) return false;

		// Write the index table
		if (WriteTag(volInfo.indexTableLength, "voli") == false) return false;
		if (WriteFile(m_OutFileHandle, volInfo.indexEntry, volInfo.indexTableLength, &numBytesWritten, NULL) == 0) return false;
		i = 0;			// Pad with 0 bytes
		if (WriteFile(m_OutFileHandle, &i, volInfo.paddedIndexTableLength - volInfo.indexTableLength, &numBytesWritten, NULL) == 0) return false;

		return true;
	}

	bool VolFile::OpenAllInputFiles(CreateVolumeInfo &volInfo)
	{
		int i;

		// Allocate space to keep track of all input files
		volInfo.indexEntry = new IndexEntry[volInfo.numFilesToPack];
		volInfo.fileHandle = new HANDLE[volInfo.numFilesToPack];
		volInfo.fileNameLength = new int[volInfo.numFilesToPack];
		volInfo.stringTableLength = 0;
		// Open all the input files and store indexing info
		for (i = 0; i < volInfo.numFilesToPack; i++)
		{
			volInfo.fileHandle[i] = CreateFileA(
				volInfo.filesToPack[i],	// filename
				GENERIC_READ,			// access mode
				0,						// share mode
				NULL,					// security attributes
				OPEN_EXISTING,			// creation disposition
				FILE_ATTRIBUTE_NORMAL,	// file attributes
				NULL);					// template
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
		int i;
		IndexEntry *tempPtr;

		if (OpenAllInputFiles(volInfo) == false) return false;// Verify input files can be read

		// Get files sizes and calculate length of string table
		for (i = 0; i < volInfo.numFilesToPack; i++)
		{
			// Store the indexing info into the index struct
			volInfo.indexEntry[i].fileSize = GetFileSize(volInfo.fileHandle[i], 0);
			volInfo.indexEntry[i].fileNameOffset = volInfo.stringTableLength;
			volInfo.indexEntry[i].compressionType = CompressionType::Uncompressed;
			// Calculate length of internal file names for string table
			volInfo.fileNameLength[i] = strlen(volInfo.internalNames[i]) + 1;
			volInfo.stringTableLength += volInfo.fileNameLength[i];
		}
		// Calculate size of index table
		volInfo.indexTableLength = volInfo.numFilesToPack * sizeof(IndexEntry);
		// Calculate the zero padded length of the string table and index table
		volInfo.paddedStringTableLength = (volInfo.stringTableLength + 7) & ~3;
		volInfo.paddedIndexTableLength = (volInfo.indexTableLength + 3) & ~3;
		volInfo.indexEntry[0].dataBlockOffset =
			volInfo.paddedStringTableLength + volInfo.paddedIndexTableLength + 32;
		// Calculate offsets to the files
		for (i = 1; i < volInfo.numFilesToPack; i++)
		{
			tempPtr = &volInfo.indexEntry[i - 1];
			volInfo.indexEntry[i].dataBlockOffset =
				(tempPtr->dataBlockOffset + tempPtr->fileSize + 11) & ~3;
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
			if (ReadFile(inputFile, buffer, VOL_WRITE_SIZE, &numBytesRead, NULL) == 0) return false;
			if (WriteFile(m_OutFileHandle, buffer, numBytesRead, &numBytesWritten, NULL) == 0) return false;
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

		return WriteFile(m_OutFileHandle, buffer, 8, &numBytesWritten, NULL) != 0;
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

		// Check for filename string table tag ("vols" tag)
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
