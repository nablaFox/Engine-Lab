#include "../include/vel_file.hpp"

namespace vel {

VelFile::VelFile() {
}

VelFile::VelFile(const char* fname) {
	openFile(fname);
}

VelFile::~VelFile() {
	closeFile();
}

bool VelFile::openFile(const char* fname, unsigned int filemode) {
	std::ios_base::openmode openMode = std::ios_base::openmode(0);

	if (filemode & FILE_READ) openMode |= std::ios::in;
	else openMode |= std::ios::out;

	if (filemode & FILE_BINARY) openMode |= std::ios::binary;

	file.open(fname, openMode);

	if (!file.is_open()) { return false; }

	return true;
}

void VelFile::closeFile() {
	if (file.is_open()) {
		file.close();
	}
}

char* VelFile::getData() {
	size_t fileLength = getFileLength();
	char* buffer = new char[fileLength + 1];
	readFileToBuffer(buffer);
	return buffer;
}

void VelFile::readFileToBuffer(char* buffer) {
	size_t fileLength = getFileLength();
	file.read(buffer, fileLength);
}

size_t VelFile::getFileLength() {
	std::streampos fsize = 0;

	file.seekg(0, std::ios::beg);
	fsize = file.tellg();
	file.seekg(0, std::ios::end);
	fsize = file.tellg() - fsize;
	file.seekg(0, std::ios::beg);

	return fsize;
}

}
