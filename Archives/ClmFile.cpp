#include "ClmFile.h"
#include "../StreamReader.h"
#include "../StreamWriter.h"
#include "../XFile.h"
#include <stdexcept>
#include <algorithm>


namespace Archives
{
	const unsigned int CLM_WRITE_SIZE = 0x00020000;

	const int RIFF = 0x46464952;	// "RIFF"
	const int WAVE = 0x45564157;	// "WAVE"
	const int FMT  = 0x20746D66;	// "fmt "
	const int DATA = 0x61746164;	// "data"

	ClmFile::ClmFile(const char *fileName) : ArchiveFile(fileName)
	{
		m_FileName = nullptr;
		m_IndexEntry = nullptr;

		// Open the file for reading
		m_FileHandle = CreateFileA(fileName,					// filename
			GENERIC_READ,				// desired access
			FILE_SHARE_READ,			// share mode
			nullptr,					// security attributes
			OPEN_EXISTING,				// creation disposition
			FILE_ATTRIBUTE_NORMAL,		// attributes
			nullptr);					// template

		if (m_FileHandle == INVALID_HANDLE_VALUE) {
			throw std::runtime_error("Could not open clm file " + std::string(fileName) + ".");
		}

		if (ReadHeader() == false) {
			throw std::runtime_error("Invalid clm header in " + std::string(fileName) + ".");
		}

		// Initialize the unknown data
		memset(m_Unknown, 0, 6);
		m_Unknown[4] = 1;
	}

	ClmFile::~ClmFile()
	{
		delete[] m_FileName;
		delete[] m_IndexEntry;

		if (m_FileHandle) {
			CloseHandle(m_FileHandle);
		}
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

		if (ReadFile(m_FileHandle, buff, 32, &numBytes, nullptr) == 0) return false;
		if (memcmp(buff, "OP2 Clump File Version 1.0\x01A\0\0\0\0", 32)) return false;

		// Read in the WaveFormatEx structure
		if (ReadFile(m_FileHandle, &m_WaveFormat, sizeof(m_WaveFormat), &numBytes, nullptr) == 0) return false;

		// Read remaining header info
		if (ReadFile(m_FileHandle, m_Unknown, 6, &numBytes, nullptr) == 0) return false;
		if (ReadFile(m_FileHandle, &m_NumberOfPackedFiles, 4, &numBytes, nullptr) == 0) return false;
		if (m_NumberOfPackedFiles < 1) return false;

		// Allocate space
		m_IndexEntry = new IndexEntry[m_NumberOfPackedFiles];
		m_FileName = new char[m_NumberOfPackedFiles][9];

		// Read Index info
		if (ReadFile(m_FileHandle, m_IndexEntry, m_NumberOfPackedFiles * sizeof(IndexEntry), &numBytes, nullptr) == 0)
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
			if (XFile::PathsAreEqual(GetInternalFileName(i), internalFileName)) {
				return i;
			}
		}

		return -1;
	}

	// Returns the size of the internal file corresponding to index
	int ClmFile::GetInternalFileSize(int index)
	{
		return m_IndexEntry[index].dataLength;
	}

	

	// Extracts the internal file corresponding to index
	void ClmFile::ExtractFile(int fileIndex, const std::string& pathOut)
	{
		WaveHeader header;
		InitializeWaveHeader(header, fileIndex);

		try
		{
			FileStreamWriter fileStreamWriter(pathOut);

			fileStreamWriter.Write((char*)&header, sizeof(header));

			// TODO: Replace with a non Windows specific solution
			// Seek to the beginning of the file data (in the .clm file)
			if (SetFilePointer(m_FileHandle, m_IndexEntry[fileIndex].dataOffset, nullptr, FILE_BEGIN) == -1) {
				return;
			}

			// Write the Wave data
			char buffer[CLM_WRITE_SIZE];
			unsigned int totalSize = 0;
			unsigned long numBytesRead = 0;
			do
			{
				// Will throw an error
				//MemoryStreamReader streamReader(buffer, CLM_WRITE_SIZE);
				//streamReader.Read((char*)m_FileHandle, CLM_WRITE_SIZE);

				// TODO: Replace with a non-Windows specific solution
				if (ReadFile(m_FileHandle, buffer, CLM_WRITE_SIZE, &numBytesRead, nullptr) == 0)
				{
					return;
				}

				if (totalSize + numBytesRead > m_IndexEntry[fileIndex].dataLength) {
					numBytesRead = m_IndexEntry[fileIndex].dataLength - totalSize;
				}

				totalSize += numBytesRead;

				fileStreamWriter.Write(buffer, numBytesRead);

			} while (numBytesRead);
		}
		catch (std::exception e)
		{
			throw std::runtime_error("Error attempting to extracted uncompressed file " + pathOut + ". Internal Error Message: " + e.what());
		}
	}

	void ClmFile::InitializeWaveHeader(WaveHeader& headerOut, int fileIndex)
	{
		headerOut.riffHeader.riffTag = RIFF;
		headerOut.riffHeader.waveTag = WAVE;
		headerOut.riffHeader.chunkSize = sizeof(headerOut.formatChunk) + 12 + m_IndexEntry[fileIndex].dataLength;

		headerOut.formatChunk.fmtTag = FMT;
		headerOut.formatChunk.formatSize = sizeof(headerOut.formatChunk.waveFormat);
		headerOut.formatChunk.waveFormat = m_WaveFormat;
		headerOut.formatChunk.waveFormat.cbSize = 0;

		headerOut.dataChunk.dataTag = DATA;
		headerOut.dataChunk.dataSize = m_IndexEntry[fileIndex].dataLength;
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

		return CreateVolume("temp.clm", filesToPack);
	}

	// Creates a new volume file with the file name volumeFileName and packs the
	// numFilesToPack files listed in the array filesToPack into the volume.
	// Automatically strips file name extensions from filesToPack. 
	// Returns nonzero if successful and zero otherwise.
	bool ClmFile::CreateVolume(std::string volumeFileName, std::vector<std::string> filesToPack)
	{
		if (filesToPack.size() < 1) {
			return false; //CLM files require at least one audio file present to properly write settings.
		}

		std::sort(filesToPack.begin(), filesToPack.end(), ComparePathFilenames);

		HANDLE outFile = nullptr;
		HANDLE *fileHandle;
		WaveFormatEx *waveFormat;
		IndexEntry *indexEntry;

		// Allocate space for all the file handles
		fileHandle = new HANDLE[filesToPack.size()];
		// Open all the files (and store file handles)
		if (OpenAllInputFiles(filesToPack, fileHandle) == false)
		{
			// Error opening files. Abort.
			delete[] fileHandle;
			return false;
		}

		// Allocate space for index entries
		indexEntry = new IndexEntry[filesToPack.size()];
		// Allocate space for the format of all wave files
		waveFormat = new WaveFormatEx[filesToPack.size()];
		// Read in all the wave headers
		if (!ReadAllWaveHeaders(filesToPack.size(), fileHandle, waveFormat, indexEntry))
		{
			// Error reading in wave headers. Abort.
			CleanUpVolumeCreate(outFile, filesToPack.size(), fileHandle, waveFormat, indexEntry);
			return false;
		}

		// Check if all wave formats are the same
		if (!CompareWaveFormats(filesToPack.size(), waveFormat))
		{
			// Not all wave formats match
			CleanUpVolumeCreate(outFile, filesToPack.size(), fileHandle, waveFormat, indexEntry);
			return false;
		}

		// Open the output file
		outFile = CreateFileA(volumeFileName.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, 0, nullptr);
		if (outFile == INVALID_HANDLE_VALUE)
		{
			// Error opening the output file
			CleanUpVolumeCreate(outFile, filesToPack.size(), fileHandle, waveFormat, indexEntry);
			return false;
		}

		std::vector<std::string> internalFileNames = GetInternalNamesFromPaths(filesToPack);
		internalFileNames = StripFileNameExtensions(internalFileNames);
		CheckSortedContainerForDuplicateNames(internalFileNames);

		// Write the volume header and copy files into the volume
		if (!WriteVolume(outFile, filesToPack.size(), fileHandle, indexEntry, internalFileNames, waveFormat))
		{
			// Error writing volume file
			CleanUpVolumeCreate(outFile, filesToPack.size(), fileHandle, waveFormat, indexEntry);
			return false;
		}

		// Close all open input files and release memory
		CleanUpVolumeCreate(outFile, filesToPack.size(), fileHandle, waveFormat, indexEntry);

		return true;
	}


	// Opens all files in filesToPack and stores the file handle in the fileHandle array
	// If any file fails to open, all already opened files are closed and the return value
	// is zero. If the function succeeds, the return value is nonzero.
	bool ClmFile::OpenAllInputFiles(std::vector<std::string> filesToPack, HANDLE *fileHandle)
	{
		// Open all the files
		for (size_t i = 0; i < filesToPack.size(); i++)
		{
			fileHandle[i] = CreateFileA(filesToPack[i].c_str(),		// filename
				GENERIC_READ,			// access mode
				0,						// share mode
				nullptr,				// security attributes
				OPEN_EXISTING,			// creation disposition
				FILE_ATTRIBUTE_NORMAL,	// file attributes
				nullptr);				// template
// Make sure the file was opened
			if (fileHandle[i] == INVALID_HANDLE_VALUE)
			{
				// Error opening files. Close already opened files and return error
				for (i--; i; i--) {
					CloseHandle(fileHandle[i]);	// Close the file
				}
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
	bool ClmFile::ReadAllWaveHeaders(int numFilesToPack, HANDLE *file, WaveFormatEx *format, IndexEntry *indexEntry)
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
			retVal = ReadFile(file[i], &header, sizeof(header), &numBytesRead, nullptr);
			if (retVal == 0 || header.riffTag != RIFF || header.waveTag != WAVE) {
				return false;		// Error reading header
			}
			// Check that the file size makes sense (matches with header chunk length + 8)
			if (header.chunkSize + 8 != GetFileSize(file[i], nullptr)) return false;

			// Read the format tag
			length = FindChunk(FMT, file[i]);
			if (length == -1) return false;		// Format chunk not found
			// Read in the wave format
			if (ReadFile(file[i], &format[i], sizeof(WaveFormatEx), &numBytesRead, nullptr) == 0) {
				return false;					// Error reading in wave format
			}
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

		fileSize = GetFileSize(file, nullptr);
		if (fileSize < 20) return -1;
		// Seek to beginning of first internal chunk (provided it exists)
		// Note: this seeks past the initial format tag (such as RIFF and WAVE)
		if (SetFilePointer(file, 12, nullptr, FILE_BEGIN) == -1) return -1;

		curPos = 12;
		do
		{
			// Read the tag
			ReadFile(file, &header, sizeof(header), &numBytesRead, nullptr);
			// Check if this is the right header
			if (header.formatTag == chunkTag) return header.length;
			// Not the right header. Skip to next header
			curPos += header.length + 8;
			if (SetFilePointer(file, curPos, nullptr, FILE_BEGIN) == -1) return -1;
		} while (curPos < fileSize);

		return -1;	// Failed to find the tag
	}

	// Closes all input files in the array fileHandle (of size numFilesToPack)
	// and releases the memory for the file handles and the wave format structures.
	void ClmFile::CleanUpVolumeCreate(HANDLE outFile,
		int numFilesToPack,
		HANDLE *fileHandle,
		WaveFormatEx *waveFormat,
		IndexEntry *indexEntry)
	{
		int i;

		// Close the output file
		if (outFile) CloseHandle(outFile);
		// Close all open input files
		for (i = 0; i < numFilesToPack; i++) {
			CloseHandle(fileHandle[i]);
		}
		// Free temporary memory
		delete[] fileHandle;
		delete[] waveFormat;
		delete[] indexEntry;
	}

	// Compares wave format structures in the array waveFormat
	// Returns true if they are all the same and false otherwise.
	bool ClmFile::CompareWaveFormats(int numFilesToPack, WaveFormatEx *waveFormat)
	{
		int i;

		for (i = 1; i < numFilesToPack; i++)
		{
			if (memcmp(&waveFormat[i], &waveFormat[0], sizeof(WaveFormatEx))) {
				return false;		// Mismatch found
			}
		}

		return true;				// They are all the same
	}

	bool ClmFile::WriteVolume(HANDLE outFile,
		int numFilesToPack,
		HANDLE *fileHandle,
		IndexEntry *entry,
		std::vector<std::string> internalNames,
		WaveFormatEx *waveFormat)
	{
#pragma pack(push, 1)
		struct SClmHeader
		{
			char textBuff[32];
			WaveFormatEx waveFormat;
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
		if (WriteFile(outFile, &header, sizeof(header), &numBytes, nullptr) == 0) return false;

		/*
		// Write the text header
		if (WriteFile(outFile, textBuff, 32, &numBytes, NULL) == 0) return false;
		// Write the wave format
		if (WriteFile(outFile, waveFormat, sizeof(WaveFormatEx), &numBytes, NULL) == 0) return false;
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
			strncpy((char*)&entry[i].fileName, internalNames[i].c_str(), 8);
			// Set the offset of the file
			entry[i].dataOffset = offset;
			offset += entry[i].dataLength;
		}

		// Write the index
		if (WriteFile(outFile, entry, numFilesToPack * sizeof(IndexEntry), &numBytes, nullptr) == 0)
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
				if (ReadFile(fileHandle[i], buff, numBytes, &numBytesRead, nullptr) == 0)
				{
					// Error reading input file
					delete[] entry;		// Release the memory for the index table
					return false;
				}
				offset += numBytesRead;
				// Write the data to the output file
				if (WriteFile(outFile, buff, numBytesRead, &numBytes, nullptr) == 0)
				{
					// Error writing output file
					delete[] entry;		// Release the memory for the index table
					return false;
				}
			} while (numBytesRead);
		}

		return true;
	}

	std::vector<std::string> ClmFile::StripFileNameExtensions(std::vector<std::string> paths)
	{
		std::vector<std::string> strippedExtensions;

		for (std::string path : paths) {
			strippedExtensions.push_back(XFile::ChangeFileExtension(path, ""));
		}

		return strippedExtensions;
	}
}
