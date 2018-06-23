#include "VolFile.h"
#include "../Streams/FileSliceReader.h"
#include "../XFile.h"
#include <stdexcept>
#include <algorithm>
#include <climits>

namespace Archives
{
	// Volume section header tags
	const std::array<char, 4> TagVOL_{ 'V', 'O', 'L', ' ' }; // Volume file tag
	const std::array<char, 4> TagVOLH{ 'v', 'o', 'l', 'h' }; // Header tag
	const std::array<char, 4> TagVOLS{ 'v', 'o', 'l', 's' }; // Filename table tag
	const std::array<char, 4> TagVOLI{ 'v', 'o', 'l', 'i' }; // Index table tag
	const std::array<char, 4> TagVBLK{ 'V', 'B', 'L', 'K' }; // Packed file tag


	VolFile::VolFile(const char *fileName) : ArchiveFile(fileName), archiveFileReader(fileName)
	{
		m_ArchiveFileSize = archiveFileReader.Length();

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
		SectionHeader sectionHeader = GetSectionHeader(fileIndex);

		return std::make_unique<FileSliceReader>(m_ArchiveFileName, 
			archiveFileReader.Position(), static_cast<uint64_t>(sectionHeader.length));
	}

	VolFile::SectionHeader VolFile::GetSectionHeader(int index)
	{
		CheckPackedFileIndexBounds(index);

		archiveFileReader.Seek(m_IndexEntries[index].dataBlockOffset);

		SectionHeader sectionHeader;
		archiveFileReader.Read(sectionHeader);

		//Volume Block
		if (sectionHeader.tag != TagVBLK) {
			throw std::runtime_error("Archive file " + m_ArchiveFileName +
				" is missing VBLK tag for requested file at index " + std::to_string(index));
		}

		return sectionHeader;
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
			// Calling GetSectionHeader moves the streamReader's position to just past the SectionHeader
			SectionHeader sectionHeader = GetSectionHeader(fileIndex);

			ArchiveUnpacker::WriteFromStream(pathOut, archiveFileReader, sectionHeader.length);
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
		PrepareHeader(volInfo);

		try {
			WriteVolume(volumeFileName, volInfo);
		}
		catch (std::exception&) {
			return false;
		}

		return true;
	}

	void VolFile::WriteVolume(const std::string& fileName, CreateVolumeInfo& volInfo) 
	{
		FileStreamWriter volWriter(fileName);

		WriteHeader(volWriter, volInfo);
		WriteFiles(volWriter, volInfo);
	}

	void VolFile::WriteFiles(StreamWriter& volWriter, CreateVolumeInfo &volInfo)
	{
		// Write each file header and contents
		for (std::size_t i = 0; i < volInfo.fileCount(); i++)
		{
			volWriter.Write(SectionHeader(TagVBLK, volInfo.indexEntries[i].fileSize));

			try {
				ArchivePacker::WriteFromStream(volWriter, *volInfo.fileStreamReaders[i], volInfo.indexEntries[i].fileSize);
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
		volWriter.Write(SectionHeader(TagVOL_, volInfo.paddedStringTableLength + volInfo.paddedIndexTableLength + 24));

		volWriter.Write(SectionHeader(TagVOLH, 0));

		// Write the string table
		volWriter.Write(SectionHeader(TagVOLS, volInfo.paddedStringTableLength));

		volWriter.Write(&volInfo.stringTableLength, sizeof(volInfo.stringTableLength));

		// Write out all internal file name strings (including NULL terminator)
		for (std::size_t i = 0; i < volInfo.fileCount(); i++) {
			// Account for the null terminator in the size.
			volWriter.Write(volInfo.internalNames[i].c_str(), volInfo.internalNames[i].size() + 1);
		}

		int padding = 0; // Pad with 0 bytes
		volWriter.Write(&padding, volInfo.paddedStringTableLength - (volInfo.stringTableLength + 4));

		// Write the index table
		volWriter.Write(SectionHeader(TagVOLI, volInfo.indexTableLength));

		volWriter.Write(volInfo.indexEntries.data(), volInfo.indexTableLength);

		volWriter.Write(&padding, volInfo.paddedIndexTableLength - volInfo.indexTableLength);
	}

	void VolFile::OpenAllInputFiles(CreateVolumeInfo &volInfo)
	{
		volInfo.fileStreamReaders.clear();

		for (const auto& filename : volInfo.filesToPack) {
			try {
				volInfo.fileStreamReaders.push_back(std::make_unique<FileStreamReader>(filename));
			}
			catch (const std::exception& e) {
				throw std::runtime_error("Error attempting to open " + filename + 
					" for reading into volume " + m_ArchiveFileName + ". Internal Error: " + e.what());
			}
		}
	}

	void VolFile::PrepareHeader(CreateVolumeInfo &volInfo)
	{
		OpenAllInputFiles(volInfo);

		volInfo.stringTableLength = 0;

		// Get file sizes and calculate length of string table
		for (std::size_t i = 0; i < volInfo.fileCount(); i++)
		{
			IndexEntry indexEntry;

			uint64_t fileSize = volInfo.fileStreamReaders[i]->Length();
			if (fileSize > UINT32_MAX) {
				throw std::runtime_error("File " + volInfo.filesToPack[i] + " is too large to fit inside a volume archive.");
			}

			indexEntry.fileSize = static_cast<uint32_t>(volInfo.fileStreamReaders[i]->Length());
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
	}

	// Reads a tag in the .vol file and returns the length of that section.
	// If tag does not match what is in the file or if the length is invalid then an error is thrown.
	uint32_t VolFile::ReadTag(std::array<char, 4> tagName)
	{
		SectionHeader tag;
		archiveFileReader.Read(tag);

		if (tag.tag != tagName) {
			throw std::runtime_error("The tag " + std::string(tagName.data(), tagName.size()) + 
				" was not found in the proper position in volume " + m_ArchiveFileName);
		}

		if (tag.padding == VolPadding::TwoByte) {
			throw std::runtime_error("The tag " + std::string(tagName.data(), tagName.size()) + 
				" from volume " + m_ArchiveFileName + 
				" uses 2 byte padding, which is not supported. Only 4 byte padding is supported.");
		}

		return tag.length;
	}

	// Reads the header structure of the .vol file and sets up indexing/structure variables
	// Returns true is the header structure is valid and false otherwise
	void VolFile::ReadVolHeader()
	{
		// Make sure file is big enough to contain header tag
		if (archiveFileReader.Length() < sizeof(SectionHeader)) {
			throw std::runtime_error("The volume file " + m_ArchiveFileName + " is not large enough to contain the 'VOL ' section header");
		}

		m_HeaderLength = ReadTag(TagVOL_);

		// Make sure the file is large enough to contain the header
		if (archiveFileReader.Length() < m_HeaderLength + sizeof(SectionHeader)) {
			throw std::runtime_error("The volume file " + m_ArchiveFileName + " is not large enough to contain the volh section header");
		}

		uint32_t volhSize = ReadTag(TagVOLH);
		if (volhSize != 0) {
			throw std::runtime_error("The length associated with tag volh is not zero in volume " + m_ArchiveFileName);
		}

		m_StringTableLength = ReadTag(TagVOLS);

		if (m_HeaderLength < m_StringTableLength + sizeof(SectionHeader) * 2 + sizeof(m_StringTableLength)) {
			throw std::runtime_error("The string table does not fit in the header of volume " + m_ArchiveFileName);
		}

		ReadStringTable();

		m_IndexTableLength = ReadTag(TagVOLI);
		m_NumberOfIndexEntries = m_IndexTableLength / sizeof(IndexEntry);

		if (m_IndexTableLength > 0) {
			m_IndexEntries.resize(m_NumberOfIndexEntries);
			archiveFileReader.Read(m_IndexEntries.data(), m_IndexTableLength);
		}

		if (m_HeaderLength < m_StringTableLength + m_IndexTableLength + 24) {
			throw std::runtime_error("The index table does not fit in the header of volume " + m_ArchiveFileName);
		}

		ReadPackedFileCount();
	}

	void VolFile::ReadStringTable()
	{
		uint32_t actualStringTableLength;
		archiveFileReader.Read(&actualStringTableLength, sizeof(actualStringTableLength));

		std::string charBuffer;
		charBuffer.resize(actualStringTableLength);
		archiveFileReader.Read(&charBuffer[0], actualStringTableLength);

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
		archiveFileReader.SeekRelative(m_StringTableLength - actualStringTableLength - 4);
	}

	void VolFile::ReadPackedFileCount()
	{
		// Count the number of valid entries
		uint32_t packedFileCount = 0;
		for (; packedFileCount < m_NumberOfIndexEntries; packedFileCount++)
		{
			// Make sure entry is valid
			if (m_IndexEntries[packedFileCount].fileNameOffset == UINT_MAX) {
				break;
			}
		}
		m_NumberOfPackedFiles = packedFileCount;
	}

	VolFile::SectionHeader::SectionHeader() {}
	VolFile::SectionHeader::SectionHeader(std::array<char, 4> tag, uint32_t length, VolPadding padding) 
		: tag(tag), length(length), padding(padding) {}
}
