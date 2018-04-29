#pragma once

namespace FileWriteFlag
{
	// Default values are Do not overwrite files, append to existing files, and write in binary format.
	enum FileWriteFlag : unsigned int
	{
		// SET:     Allow overwritting the file if it exists (mutually exclusive from Append).
		// NOT SET: Do not allow overwritting an exisitng file.
		Overwrite = 0b0000'0001,

		// SET:     Do not allow a new file to be created (Mutually exclusive from Overwrite).
		// NOT SET: Allow new file creation. Overwrite must also be set.
		DoNotCreate = 0b0000'0010,

		// SET:     If file exists, default to truncating file to zero bytes
		// NOT SET: If file exists, default to writing at end of file (Append).
		Truncate = 0b0000'0100,

		// SET:     Specifies text file format.
		// NOT SET: Specified binary file format.
		Text = 0b0000'1000
	};
}
