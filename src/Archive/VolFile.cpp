#include "VolFile.h"
#include "../Stream/SliceReader.h"
#include "../XFile.h"
#include <stdexcept>
#include <algorithm>
#include <climits>
#include <typeinfo>

namespace Archive
{
	// Volume section header tags
	constexpr auto TagVOL_ = MakeTag("VOL "); // Volume file tag
	constexpr auto TagVOLH = MakeTag("volh"); // Header tag
	constexpr auto TagVOLS = MakeTag("vols"); // Filename table tag
	constexpr auto TagVOLI = MakeTag("voli"); // Index table tag
	constexpr auto TagVBLK = MakeTag("VBLK"); // Packed file tag


	VolFile::VolFile(const std::string& filename) : ArchiveFile(filename), archiveFileReader(filename)
	{
		m_ArchiveFileSize = archiveFileReader.Length();

		ReadVolHeader();
	}

	VolFile::~VolFile() { }



	std::string VolFile::GetName(std::size_t index)
	{
		VerifyIndexInBounds(index);

		return m_StringTable[index];
	}

	CompressionType VolFile::GetCompressionCode(std::size_t index)
	{
		VerifyIndexInBounds(index);

		return m_IndexEntries[index].compressionType;
	}

	uint32_t VolFile::GetSize(std::size_t index)
	{
		VerifyIndexInBounds(index);

		return m_IndexEntries[index].fileSize;
	}



	int VolFile::GetFileOffset(std::size_t index)
	{
		return m_IndexEntries[index].dataBlockOffset + sizeof(SectionHeader);
	}

	int VolFile::GetFilenameOffset(std::size_t index)
	{
		return m_IndexEntries[index].filenameOffset;
	}

	std::unique_ptr<Stream::BidirectionalReader> VolFile::OpenStream(std::size_t index)
	{
		SectionHeader sectionHeader = GetSectionHeader(index);

		return std::make_unique<Stream::FileSliceReader>(archiveFileReader.Slice(archiveFileReader.Position(), static_cast<uint64_t>(sectionHeader.length)));
	}

	VolFile::SectionHeader VolFile::GetSectionHeader(std::size_t index)
	{
		VerifyIndexInBounds(index);

		archiveFileReader.Seek(m_IndexEntries[index].dataBlockOffset);

		SectionHeader sectionHeader;
		archiveFileReader.Read(sectionHeader);

		//Volume Block
		if (sectionHeader.tag != TagVBLK) {
			throw std::runtime_error("Archive file " + m_ArchiveFilename +
				" is missing VBLK tag for requested file at index " + std::to_string(index));
		}

		return sectionHeader;
	}

	// Extracts the internal file at the given index to the filename.
	void VolFile::ExtractFile(std::size_t index, const std::string& pathOut)
	{
		VerifyIndexInBounds(index);
		const auto& indexEntry = m_IndexEntries[index];

		if (indexEntry.compressionType == CompressionType::Uncompressed)
		{
			ExtractFileUncompressed(index, pathOut);
		}
		else if (indexEntry.compressionType == CompressionType::LZH)
		{
			ExtractFileLzh(index, pathOut);
		}
		else {
			throw std::runtime_error("Compression type is not supported.");
		}
	}

	void VolFile::ExtractFileUncompressed(std::size_t index, const std::string& pathOut)
	{
		try
		{
			// Calling GetSectionHeader moves the streamReader's position to just past the SectionHeader
			SectionHeader sectionHeader = GetSectionHeader(index);
			auto slice = archiveFileReader.Slice(sectionHeader.length);
			Stream::FileWriter fileStreamWriter(pathOut);
			fileStreamWriter.Write(slice);
		}
		catch (const std::exception& e)
		{
			throw std::runtime_error("Error attempting to extracted uncompressed file " + pathOut + ". Internal Error Message: " + e.what());
		}
	}

	void VolFile::ExtractFileLzh(std::size_t index, const std::string& pathOut)
	{
		try
		{
			// Calling GetSectionHeader moves the streamReader's position to just past the SectionHeader
			SectionHeader sectionHeader = GetSectionHeader(index);

			// Load data into temporary memory buffer
			std::size_t length = sectionHeader.length;
			std::vector<uint8_t> buffer(length);
			archiveFileReader.Read(buffer);

			HuffLZ decompressor(BitStreamReader(buffer.data(), length));

			Stream::FileWriter fileStreamWriter(pathOut);

			do
			{
				const void *buffer = decompressor.GetInternalBuffer(&length);
				fileStreamWriter.Write(buffer, length);
			} while (length);
		}
		catch (const std::exception& e)
		{
			throw std::runtime_error("Error attempting to extract LZH compressed file " + pathOut + ". Internal Error Message: " + e.what());
		}
	}



	void VolFile::CreateArchive(const std::string& volumeFilename, std::vector<std::string> filesToPack)
	{
		// Sort files alphabetically based on the filename only (not including the full path).
		// Packed files must be locatable by a binary search of their filename.
		std::sort(filesToPack.begin(), filesToPack.end(), ComparePathFilenames);

		CreateVolumeInfo volInfo;

		volInfo.filesToPack = filesToPack;
		volInfo.names = GetNamesFromPaths(filesToPack);

		// Allowing duplicate names when packing may cause unintended results during binary search and file extraction.
		VerifySortedContainerHasNoDuplicateNames(volInfo.names);

		// Open input files and prepare header and indexing info
		PrepareHeader(volInfo, volumeFilename);

		WriteVolume(volumeFilename, volInfo);
	}

	void VolFile::WriteVolume(const std::string& filename, CreateVolumeInfo& volInfo)
	{
		for (const auto& path : volInfo.filesToPack) {
			if (XFile::PathsAreEqual(filename, path)) {
				throw std::runtime_error("Cannot include a volume being overwritten in new volume " + filename);
			}
		}

		Stream::FileWriter volWriter(filename);

		WriteHeader(volWriter, volInfo);
		WriteFiles(volWriter, volInfo);
	}

	void VolFile::WriteFiles(Stream::Writer& volWriter, CreateVolumeInfo &volInfo)
	{
		// Write each file header and contents
		for (std::size_t i = 0; i < volInfo.fileCount(); ++i)
		{
			volWriter.Write(SectionHeader(TagVBLK, volInfo.indexEntries[i].fileSize));

			try {
				volWriter.Write(*volInfo.fileStreamReaders[i]);
				int padding = 0;

				// Add padding after the file, ensuring it ends on a 4 byte boundary
				// Use a bitmask to quickly calculate the modulo 4 (remainder) of fileSize
				volWriter.Write(&padding, (-volInfo.indexEntries[i].fileSize) & 3);
			}
			catch (const std::exception& e) {
				throw std::runtime_error("Unable to pack file " + volInfo.names[i] + ". Internal error: " + e.what());
			}
		}
	}

	void VolFile::WriteHeader(Stream::Writer& volWriter, const CreateVolumeInfo &volInfo)
	{
		// Write the header
		volWriter.Write(SectionHeader(TagVOL_, volInfo.paddedStringTableLength + volInfo.paddedIndexTableLength + 24));

		volWriter.Write(SectionHeader(TagVOLH, 0));

		// Write the string table
		volWriter.Write(SectionHeader(TagVOLS, volInfo.paddedStringTableLength));

		volWriter.Write(volInfo.stringTableLength);

		// Write out all internal file name strings (including NULL terminator)
		for (std::size_t i = 0; i < volInfo.fileCount(); ++i) {
			// Account for the null terminator in the size.
			volWriter.Write(volInfo.names[i].c_str(), volInfo.names[i].size() + 1);
		}

		int padding = 0; // Pad with 0 bytes
		volWriter.Write(&padding, volInfo.paddedStringTableLength - (volInfo.stringTableLength + 4));

		// Write the index table
		volWriter.Write(SectionHeader(TagVOLI, volInfo.indexTableLength));

		volWriter.Write(volInfo.indexEntries.data(), volInfo.indexTableLength);

		volWriter.Write(&padding, volInfo.paddedIndexTableLength - volInfo.indexTableLength);
	}

	void VolFile::OpenAllInputFiles(CreateVolumeInfo &volInfo, const std::string& volumeFilename)
	{
		volInfo.fileStreamReaders.clear();

		for (const auto& filename : volInfo.filesToPack) {
			try {
				volInfo.fileStreamReaders.push_back(std::make_unique<Stream::FileReader>(filename));
			}
			catch (const std::exception& e) {
				throw std::runtime_error("Error attempting to open " + filename +
					" for reading into volume " + volumeFilename + ". Internal Error: " + e.what());
			}
		}
	}

	void VolFile::PrepareHeader(CreateVolumeInfo &volInfo, const std::string& volumeFilename)
	{
		OpenAllInputFiles(volInfo, volumeFilename);

		volInfo.stringTableLength = 0;

		// Get file sizes and calculate length of string table
		for (std::size_t i = 0; i < volInfo.fileCount(); ++i)
		{
			IndexEntry indexEntry;

			uint64_t fileSize = volInfo.fileStreamReaders[i]->Length();
			if (fileSize > UINT32_MAX) {
				throw std::runtime_error("File " + volInfo.filesToPack[i] +
					" is too large to fit inside a volume archive. Writing volume " + volumeFilename + " aborted.");
			}

			indexEntry.fileSize = static_cast<uint32_t>(fileSize);
			indexEntry.filenameOffset = volInfo.stringTableLength;
			indexEntry.compressionType = CompressionType::Uncompressed;

			volInfo.indexEntries.push_back(indexEntry);

			// Add length of internal filename plus null terminator to string table length.
			if (static_cast<uint64_t>(volInfo.stringTableLength) + volInfo.names[i].size() + 1 > UINT32_MAX) {
				throw std::runtime_error("String table length is too long to create volume " + volumeFilename);
			}

			volInfo.stringTableLength += static_cast<uint32_t>(volInfo.names[i].size()) + 1;
		}

		// Calculate size of index table
		if (static_cast<uint64_t>(volInfo.fileCount()) * sizeof(IndexEntry) > UINT32_MAX) {
			throw std::runtime_error("Index table length is too long to create volume " + volumeFilename);
		}
		volInfo.indexTableLength = static_cast<uint32_t>(volInfo.fileCount()) * sizeof(IndexEntry);

		// Calculate the zero padded length of the string table and index table
		volInfo.paddedStringTableLength = (volInfo.stringTableLength + 7) & ~3;
		volInfo.paddedIndexTableLength = (volInfo.indexTableLength + 3) & ~3;

		if (volInfo.indexEntries.size() == 0) {
			return;
		}

		volInfo.indexEntries[0].dataBlockOffset = volInfo.paddedStringTableLength + volInfo.paddedIndexTableLength + 32;

		// Calculate offsets to the files
		for (std::size_t i = 1; i < volInfo.fileCount(); ++i)
		{
			const IndexEntry& previousIndex = volInfo.indexEntries[i - 1];
			volInfo.indexEntries[i].dataBlockOffset = (previousIndex.dataBlockOffset + previousIndex.fileSize + 11) & ~3;
		}
	}

	// Reads a tag in the .vol file and returns the length of that section.
	// If tag does not match what is in the file or if the length is invalid then an error is thrown.
	uint32_t VolFile::ReadTag(Tag tagName)
	{
		SectionHeader tag;
		archiveFileReader.Read(tag);

		if (tag.tag != tagName) {
			throw std::runtime_error("The tag " + tagName +
				" was not found in the proper position in volume " + m_ArchiveFilename);
		}

		if (tag.padding == VolPadding::TwoByte) {
			throw std::runtime_error("The tag " + tagName +
				" from volume " + m_ArchiveFilename +
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
			throw std::runtime_error("The volume file " + m_ArchiveFilename + " is not large enough to contain the 'VOL ' section header");
		}

		m_HeaderLength = ReadTag(TagVOL_);

		// Make sure the file is large enough to contain the header
		if (archiveFileReader.Length() < m_HeaderLength + sizeof(SectionHeader)) {
			throw std::runtime_error("The volume file " + m_ArchiveFilename + " is not large enough to contain the volh section header");
		}

		uint32_t volhSize = ReadTag(TagVOLH);
		if (volhSize != 0) {
			throw std::runtime_error("The length associated with tag volh is not zero in volume " + m_ArchiveFilename);
		}

		m_StringTableLength = ReadTag(TagVOLS);

		if (m_HeaderLength < m_StringTableLength + sizeof(SectionHeader) * 2 + sizeof(m_StringTableLength)) {
			throw std::runtime_error("The string table does not fit in the header of volume " + m_ArchiveFilename);
		}

		ReadStringTable();

		m_IndexTableLength = ReadTag(TagVOLI);
		m_IndexEntryCount = m_IndexTableLength / sizeof(IndexEntry);

		if (m_IndexTableLength > 0) {
			m_IndexEntries.resize(m_IndexEntryCount);
			archiveFileReader.Read(m_IndexEntries.data(), m_IndexTableLength);
		}

		if (m_HeaderLength < m_StringTableLength + m_IndexTableLength + 24) {
			throw std::runtime_error("The index table does not fit in the header of volume " + m_ArchiveFilename);
		}

		CountValidEntries();
	}

	void VolFile::ReadStringTable()
	{
		uint32_t actualStringTableLength;
		archiveFileReader.Read(actualStringTableLength);

		std::string charBuffer;
		charBuffer.resize(actualStringTableLength);
		archiveFileReader.Read(charBuffer);

		m_StringTable.push_back("");
		for (std::size_t i = 0; i < charBuffer.size(); ++i)
		{
			if (charBuffer[i] == '\0') {
				m_StringTable.push_back("");
				continue;
			}

			m_StringTable[m_StringTable.size() - 1].push_back(charBuffer[i]);
		}

		m_StringTable.erase(m_StringTable.begin() + m_StringTable.size() - 1);

		// Seek to the end of padding at end of StringTable
		archiveFileReader.SeekForward(m_StringTableLength - actualStringTableLength - 4);
	}

	void VolFile::CountValidEntries()
	{
		// Count the number of valid entries
		uint32_t packedFileCount = 0;
		for (; packedFileCount < m_IndexEntryCount; ++packedFileCount)
		{
			// Make sure entry is valid
			if (m_IndexEntries[packedFileCount].filenameOffset == UINT_MAX) {
				break;
			}
		}
		m_Count = packedFileCount;
	}

	VolFile::SectionHeader::SectionHeader() {}
	VolFile::SectionHeader::SectionHeader(Tag tag, uint32_t length, VolPadding padding)
		: tag(tag), length(length), padding(padding) {}
}
