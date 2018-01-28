#include "ClmFile.h"
#include "../XFile.h"
#include <stdexcept>
#include <string>

namespace Archives
{
	const unsigned int CLM_WRITE_SIZE = 0x00020000;

	const int RIFF = 0x46464952;	// "RIFF"
	const int WAVE = 0x45564157;	// "WAVE"
	const int FMT  = 0x20746D66;	// "fmt "
	const int DATA = 0x61746164;	// "data"

	ClmFile::ClmFile(const char *fileName) : ArchiveFile(fileName)
	{
		m_FileName = NULL;
		m_IndexEntry = NULL;

		// Open the file for reading
		m_FileHandle = CreateFileA(fileName,					// filename
			GENERIC_READ,				// desired access
			FILE_SHARE_READ,			// share mode
			NULL,						// security attributes
			OPEN_EXISTING,			// creation disposition
			FILE_ATTRIBUTE_NORMAL,	// attributes
			NULL);					// template

		if (m_FileHandle == INVALID_HANDLE_VALUE) 
			throw std::runtime_error("Could not open clm file " + std::string(fileName) + ".");
		
		if (ReadHeader() == false) 
			throw std::runtime_error("Invalid clm header in " + std::string(fileName) + ".");

		// Initialize the unknown data
		memset(m_Unknown, 0, 6);
		m_Unknown[4] = 1;
	}

	ClmFile::~ClmFile()
	{
		delete[] m_FileName;
		delete[] m_IndexEntry;

		if (m_FileHandle) 
			CloseHandle(m_FileHandle);
	}


	// Reads in the header when the volume is first opened and does some
	// basic error checking on the header.
	// Returns nonzero if the header was read successfully and is intact
	// Returns zero if an error occured
	bool ClmFile::ReadHeader()
	{
		char buff[32];
		unsigned long numBytes;
		int i;

		if (ReadFile(m_FileHandle, buff, 32, &numBytes, NULL) == 0) return false;
		if (memcmp(buff, "OP2 Clump File Version 1.0\x01A\0\0\0\0", 32)) return false;

		// Read in the WAVEFORMATEX structure
		if (ReadFile(m_FileHandle, &m_WaveFormat, sizeof(m_WaveFormat), &numBytes, NULL) == 0) return false;

		// Read remaining header info
		if (ReadFile(m_FileHandle, m_Unknown, 6, &numBytes, NULL) == 0) return false;
		if (ReadFile(m_FileHandle, &m_NumberOfPackedFiles, 4, &numBytes, NULL) == 0) return false;
		if (m_NumberOfPackedFiles < 1) return false;

		// Allocate space
		m_IndexEntry = new IndexEntry[m_NumberOfPackedFiles];
		m_FileName = new char[m_NumberOfPackedFiles][9];

		// Read Index info
		if (ReadFile(m_FileHandle, m_IndexEntry, m_NumberOfPackedFiles * sizeof(IndexEntry), &numBytes, NULL) == 0)
		{
			delete[] m_FileName;
			delete[] m_IndexEntry;
			return false;
		}
		// Copy file names
		for (i = 0; i < m_NumberOfPackedFiles; i++)
		{
			memcpy(m_FileName[i], &m_IndexEntry[i].fileName, 8);
			m_FileName[i][8] = 0;	// NULL terminate it
		}

		return true;
	}



	// Returns the internal file name of the internal file corresponding to index
	const char* ClmFile::GetInternalFileName(int index)
	{
		return m_FileName[index];
	}

	int ClmFile::GetInternalFileIndex(const char *internalFileName)
	{
		for (int i = 0; i < GetNumberOfPackedFiles(); ++i)
		{
			if (XFile::pathsAreEqual(GetInternalFileName(i), internalFileName))
				return i;
		}

		return -1;
	}

	// Returns the size of the internal file corresponding to index
	int ClmFile::GetInternalFileSize(int index)
	{
		return m_IndexEntry[index].dataLength;
	}

	// Extracts the internal file corresponding to index into the file fileName
	// Returns nonzero if successful and zero otherwise
	int ClmFile::ExtractFile(int index, const char *fileName)
	{
#pragma pack(push, 1)
		struct RiffHeader
		{
			int riffTag;
			int chunkSize;
			int waveTag;
		};
		struct FormatChunk
		{
			int fmtTag;
			int formatSize;
			WAVEFORMATEX waveFormat;
		};
		struct DataChunk
		{
			int dataTag;
			int dataSize;
		};
#pragma pack(pop)

		unsigned long numBytes;
		unsigned long numBytesRead;
		unsigned int totalSize;
		HANDLE outFile;
		RiffHeader riffHeader;
		FormatChunk formatChunk;
		DataChunk dataChunk;

		// Open the file
		outFile = CreateFileA(fileName,				// filename
			GENERIC_WRITE,			// desired access
			0,						// share mode
			NULL,					// security attributes
			CREATE_ALWAYS,			// creation disposition
			FILE_ATTRIBUTE_NORMAL,	// attributes
			NULL);					// template
// Check for errors opening the file
		if (outFile == INVALID_HANDLE_VALUE) return false;

		// Write the output
		// ----------------
		// Prepare the Wave file header
		riffHeader.riffTag = RIFF;
		riffHeader.waveTag = WAVE;
		riffHeader.chunkSize = sizeof(formatChunk) + 12 + m_IndexEntry[index].dataLength;
		formatChunk.fmtTag = FMT;
		formatChunk.formatSize = sizeof(formatChunk.waveFormat);
		formatChunk.waveFormat = m_WaveFormat;
		formatChunk.waveFormat.cbSize = 0;
		dataChunk.dataTag = DATA;
		dataChunk.dataSize = m_IndexEntry[index].dataLength;
		// Write the Wave file header
		if (WriteFile(outFile, &riffHeader, sizeof(riffHeader), &numBytes, NULL) == 0)
		{
			CloseHandle(outFile);
			return false;
		}
		if (WriteFile(outFile, &formatChunk, sizeof(formatChunk), &numBytes, NULL) == 0)
		{
			CloseHandle(outFile);
			return false;
		}
		if (WriteFile(outFile, &dataChunk, sizeof(dataChunk), &numBytes, NULL) == 0)
		{
			CloseHandle(outFile);
			return false;
		}
		// Seek to the beginning of the file data (in the .clm file)
		if (SetFilePointer(m_FileHandle, m_IndexEntry[index].dataOffset, NULL, FILE_BEGIN) == -1)
		{
			CloseHandle(outFile);
			return false;
		}
		// Write the Wave data
		char buff[CLM_WRITE_SIZE];
		totalSize = 0;
		do
		{
			// Read the input data
			if (ReadFile(m_FileHandle, buff, CLM_WRITE_SIZE, &numBytesRead, NULL) == 0)
			{
				CloseHandle(outFile);
				return false;
			}
			if (totalSize + numBytesRead > m_IndexEntry[index].dataLength)
				numBytesRead = m_IndexEntry[index].dataLength - totalSize;
			totalSize += numBytesRead;
			if (WriteFile(outFile, buff, numBytesRead, &numBytes, NULL) == 0)
			{
				CloseHandle(outFile);
				return false;
			}
		} while (numBytesRead);

		// Close the file
		CloseHandle(outFile);

		return true;
	}

	std::unique_ptr<SeekableStreamReader> ClmFile::OpenSeekableStreamReader(const char* internalFileName)
	{
		int fileIndex = GetInternalFileIndex(internalFileName);

		if (fileIndex < 0)
			throw std::runtime_error("File does not exist in Archive.");

		return OpenSeekableStreamReader(fileIndex);
	}

	std::unique_ptr<SeekableStreamReader> ClmFile::OpenSeekableStreamReader(int fileIndex)
	{
		throw std::logic_error("OpenSeekableStreamReader not yet implemented for Clm files.");
	}

	// Repacks the volume using the same files as are specified by the internal file names
	// Returns nonzero if successful and zero otherwise
	bool ClmFile::Repack()
	{
		char **filesToPack;
		const char **internalNames;
		int i;
		bool bRet;

		// Allocate space for file names
		filesToPack = new char*[m_NumberOfPackedFiles];
		internalNames = new const char*[m_NumberOfPackedFiles];

		// Make a list of file names
		for (i = 0; i < m_NumberOfPackedFiles; i++)
		{
			internalNames[i] = GetInternalFileName(i);
			filesToPack[i] = new char[13];
			strcpy(filesToPack[i], internalNames[i]);
			strcat(filesToPack[i], ".wav");
		}

		// Create the volume
		bRet = CreateVolume("temp.clm", m_NumberOfPackedFiles, (const char**)filesToPack, internalNames);

		// Clean up
		for (i = 0; i < m_NumberOfPackedFiles; i++)
			delete filesToPack[i];
		delete[] internalNames;
		delete[] filesToPack;

		return bRet;
	}

	// Creates a new volume file with the file name volumeFileName and packs the
	// numFilesToPack files listed in the array filesToPack into the volume.
	// The internal names of these files are given in the array internalNames.
	// Returns nonzero if successful and zero otherwise
	bool ClmFile::CreateVolume(const char *volumeFileName, int numFilesToPack,
		const char **filesToPack, const char **internalNames)
	{
		// Make sure files are specified properly.
		if (numFilesToPack < 1)
			return false; //CLM files require at least one audio file present in order to properly write settings.
		
		if (filesToPack == NULL || internalNames == NULL)
			return false;

		HANDLE outFile = NULL;
		HANDLE *fileHandle;
		WAVEFORMATEX *waveFormat;
		IndexEntry *indexEntry;

		// Allocate space for all the file handles
		fileHandle = new HANDLE[numFilesToPack];
		// Open all the files (and store file handles)
		if (OpenAllInputFiles(numFilesToPack, filesToPack, fileHandle) == false)
		{
			// Error opening files. Abort.
			delete[] fileHandle;
			return false;
		}

		// Allocate space for index entries
		indexEntry = new IndexEntry[numFilesToPack];
		// Allocate space for the format of all wave files
		waveFormat = new WAVEFORMATEX[numFilesToPack];
		// Read in all the wave headers
		if (!ReadAllWaveHeaders(numFilesToPack, fileHandle, waveFormat, indexEntry))
		{
			// Error reading in wave headers. Abort.
			CleanUpVolumeCreate(outFile, numFilesToPack, fileHandle, waveFormat, indexEntry);
			return false;
		}

		// Check if all wave formats are the same
		if (!CompareWaveFormats(numFilesToPack, waveFormat))
		{
			// Not all wave formats match
			CleanUpVolumeCreate(outFile, numFilesToPack, fileHandle, waveFormat, indexEntry);
			return false;
		}

		// Open the output file
		outFile = CreateFileA(volumeFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL); //CREATE_ALWAYS was CREATE_NEW
		if (outFile == INVALID_HANDLE_VALUE)
		{
			// Error opening the output file
			CleanUpVolumeCreate(outFile, numFilesToPack, fileHandle, waveFormat, indexEntry);
			return false;
		}

		// Write the volume header and copy files into the volume
		if (!WriteVolume(outFile, numFilesToPack, fileHandle, indexEntry, internalNames, waveFormat))
		{
			// Error writing volume file
			CleanUpVolumeCreate(outFile, numFilesToPack, fileHandle, waveFormat, indexEntry);
			return false;
		}

		// Close all open input files and release memory
		CleanUpVolumeCreate(outFile, numFilesToPack, fileHandle, waveFormat, indexEntry);

		return true;
	}


	// Opens all files in filesToPack and stores the file handle in the fileHandle array
	// If any file fails to open, all already opened files are closed and the return value
	// is zero. If the function succeeds, the return value is nonzero.
	bool ClmFile::OpenAllInputFiles(int numFilesToPack, const char **filesToPack, HANDLE *fileHandle)
	{
		int i;

		// Open all the files
		for (i = 0; i < numFilesToPack; i++)
		{
			fileHandle[i] = CreateFileA(filesToPack[i],			// filename
				GENERIC_READ,			// access mode
				0,						// share mode
				NULL,					// security attributes
				OPEN_EXISTING,			// creation disposition
				FILE_ATTRIBUTE_NORMAL,	// file attributes
				NULL);					// template
// Make sure the file was opened
			if (fileHandle[i] == INVALID_HANDLE_VALUE)
			{
				// Error opening files. Close already opened files and return error
				for (i--; i; i--)
					CloseHandle(fileHandle[i]);	// Close the file
				return false;
			}
		}

		// All files opened successfully
		return true;
	}

	// Reads the beginning of the file and verifies that the file is a WAVE file and finds
	// the format structure and start of data. The format is stored in the format array
	// and the current file pointer is set to the start of the data chunk.
	// Returns nonzero if successful and zero otherwise.
	// Note: This function assumes that all file pointers are initially set to the beginning
	//  of the file. When reading the wave file header, it does not seek to the file start.
	bool ClmFile::ReadAllWaveHeaders(int numFilesToPack, HANDLE *file, WAVEFORMATEX *format, IndexEntry *indexEntry)
	{
#pragma pack(push, 1)
		struct RiffHeader
		{
			int riffTag;
			unsigned int chunkSize;
			int waveTag;
		};
#pragma pack(pop)

		unsigned long numBytesRead;
		RiffHeader header;
		int i, length, retVal;

		// Read in all the headers and find start of data
		for (i = 0; i < numFilesToPack; i++)
		{
			// Read the file header
			retVal = ReadFile(file[i], &header, sizeof(header), &numBytesRead, NULL);
			if (retVal == 0 || header.riffTag != RIFF || header.waveTag != WAVE)
				return false;		// Error reading header
			// Check that the file size makes sense (matches with header chunk length + 8)
			if (header.chunkSize + 8 != GetFileSize(file[i], NULL)) return false;

			// Read the format tag
			length = FindChunk(FMT, file[i]);
			if (length == -1) return false;		// Format chunk not found
			// Read in the wave format
			if (ReadFile(file[i], &format[i], sizeof(WAVEFORMATEX), &numBytesRead, NULL) == 0)
				return false;					// Error reading in wave format
			format[i].cbSize = 0;

			// Find the start of the data
			length = FindChunk(DATA, file[i]);
			if (length == -1) return false;		// Data chunk not found
			// Store the length of the wave data
			indexEntry[i].dataLength = length;
			// Note: Current file pointer is set to the start of the wave data
		}

		return true;
	}

	// Searches through the wave file to find the given chunk length
	// The current file pointer is set the the first byte after the chunk header
	// Returns the chunk length if found or -1 otherwise
	int ClmFile::FindChunk(int chunkTag, HANDLE file)
	{
#pragma pack(push, 1)
		struct ChunkHeader
		{
			int formatTag;
			int length;
		};
#pragma pack(pop)

		unsigned long numBytesRead;
		ChunkHeader header;
		int curPos;
		int fileSize;

		fileSize = GetFileSize(file, NULL);
		if (fileSize < 20) return -1;
		// Seek to beginning of first internal chunk (provided it exists)
		// Note: this seeks past the initial format tag (such as RIFF and WAVE)
		if (SetFilePointer(file, 12, NULL, FILE_BEGIN) == -1) return -1;

		curPos = 12;
		do
		{
			// Read the tag
			ReadFile(file, &header, sizeof(header), &numBytesRead, NULL);
			// Check if this is the right header
			if (header.formatTag == chunkTag) return header.length;
			// Not the right header. Skip to next header
			curPos += header.length + 8;
			if (SetFilePointer(file, curPos, NULL, FILE_BEGIN) == -1) return -1;
		} while (curPos < fileSize);

		return -1;	// Failed to find the tag
	}

	// Closes all input files in the array fileHandle (of size numFilesToPack)
	// and releases the memory for the file handles and the wave format structures.
	void ClmFile::CleanUpVolumeCreate(HANDLE outFile,
		int numFilesToPack,
		HANDLE *fileHandle,
		WAVEFORMATEX *waveFormat,
		IndexEntry *indexEntry)
	{
		int i;

		// Close the output file
		if (outFile) CloseHandle(outFile);
		// Close all open input files
		for (i = 0; i < numFilesToPack; i++)
			CloseHandle(fileHandle[i]);
		// Free temporary memory
		delete[] fileHandle;
		delete[] waveFormat;
		delete[] indexEntry;
	}

	// Compares wave format structures in the array waveFormat
	// Returns true if they are all the same and false otherwise.
	bool ClmFile::CompareWaveFormats(int numFilesToPack, WAVEFORMATEX *waveFormat)
	{
		int i;

		for (i = 1; i < numFilesToPack; i++)
		{
			if (memcmp(&waveFormat[i], &waveFormat[0], sizeof(WAVEFORMATEX)))
				return false;		// Mismatch found
		}

		return true;				// They are all the same
	}

	bool ClmFile::WriteVolume(HANDLE outFile,
		int numFilesToPack,
		HANDLE *fileHandle,
		IndexEntry *entry,
		const char **internalName,
		WAVEFORMATEX *waveFormat)
	{
#pragma pack(push, 1)
		struct SClmHeader
		{
			char textBuff[32];
			WAVEFORMATEX waveFormat;
			char unknown[6];
			int numIndexEntries;
		};
#pragma pack(pop)

		char buff[CLM_WRITE_SIZE];
		SClmHeader header;
		//	char textBuff[32] = "OP2 Clump File Version 1.0\x01A\0\0\0\0";
		unsigned long numBytesRead;
		unsigned long numBytes;
		int i;
		int offset;

		memcpy(&header.textBuff, "OP2 Clump File Version 1.0\x01A\0\0\0\0", 32);
		header.waveFormat = *waveFormat;
		memcpy(header.unknown, m_Unknown, 6);
		header.numIndexEntries = numFilesToPack;

		// Write the header
		if (WriteFile(outFile, &header, sizeof(header), &numBytes, NULL) == 0) return false;

		/*
		// Write the text header
		if (WriteFile(outFile, textBuff, 32, &numBytes, NULL) == 0) return false;
		// Write the wave format
		if (WriteFile(outFile, waveFormat, sizeof(WAVEFORMATEX), &numBytes, NULL) == 0) return false;
		// Write the unknown bytes
		if (WriteFile(outFile, m_Unknown, 6, &numBytes, NULL) == 0) return false;
		// Write the number of internal files
		if (WriteFile(outFile, &m_NumberOfPackedFiles, 4, &numBytes, NULL) == 0) return false;
		*/

		// Prepare the index entries
		offset = sizeof(header) + numFilesToPack * sizeof(IndexEntry);
		for (i = 0; i < numFilesToPack; i++)
		{
			// Copy the filename into the entry
			strncpy((char*)&entry[i].fileName, internalName[i], 8);
			// Set the offset of the file
			entry[i].dataOffset = offset;
			offset += entry[i].dataLength;
		}

		// Write the index
		if (WriteFile(outFile, entry, numFilesToPack * sizeof(IndexEntry), &numBytes, NULL) == 0)
		{
			// Error writing index table
			delete[] entry;		// Release the memory for the index table
			return false;
		}

		// Copy the files into the volume
		for (i = 0; i < numFilesToPack; i++)
		{
			offset = 0;
			do
			{
				numBytes = CLM_WRITE_SIZE;
				if (offset + numBytes > entry[i].dataLength) numBytes = entry[i].dataLength - offset;
				// Read the input file
				if (ReadFile(fileHandle[i], buff, numBytes, &numBytesRead, NULL) == 0)
				{
					// Error reading input file
					delete[] entry;		// Release the memory for the index table
					return false;
				}
				offset += numBytesRead;
				// Write the data to the output file
				if (WriteFile(outFile, buff, numBytesRead, &numBytes, NULL) == 0)
				{
					// Error writing output file
					delete[] entry;		// Release the memory for the index table
					return false;
				}
			} while (numBytesRead);
		}

		return true;
	}
}
