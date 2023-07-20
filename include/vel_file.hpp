#pragma once

#include <fstream>

namespace vel {

class VelFile {
public:
	enum FILE_MODE {
		FILE_WRITE = 0x000000,
		FILE_TEXT = 0x000000,

		FILE_READ = 0x000001,
		FILE_BINARY = 0x000002,
	};

public:
	VelFile();
	VelFile(const char* fname);
	~VelFile();

	bool openFile(const char* fname, unsigned int filemode = FILE_READ);
	void closeFile();

	size_t getFileLength();
	char* getData();

	void readFileToBuffer(char* buffer);

	bool isOpen() { return file.is_open(); }

private:
	unsigned int filemode;
	std::fstream file;
};

}
