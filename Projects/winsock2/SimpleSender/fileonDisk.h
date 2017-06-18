#pragma once

#include <windows.h>
#include "bitlib.h"

class memoryMappedFile
{
public:
	memoryMappedFile() : ptr(NULL) {}
	~memoryMappedFile() { close(); }

	bool create(const char* filename, LARGE_INTEGER size);
	bool create(const char* filename, int size)
	{
		LARGE_INTEGER li;
		li.QuadPart = size;
		return create(filename, li);
	};
	bool open(const char* filename);
	void close();
	bool write(LARGE_INTEGER pos, const char* buff, int bufflen); // resizes file if required
	bool write(unsigned int pos, const char* buff, int bufflen);

	int getlastError() { return GetLastError(); }
	char* getPtr() { return ptr; }
	LARGE_INTEGER getSize() { return fileSize; }

protected:
	bool resize(LARGE_INTEGER newSize);
	HANDLE hFile, hMapFile;
	char* ptr;
	LARGE_INTEGER fileSize;
};


class fileonDisk
{
public:
	fileonDisk(int _buffSize, const char* _root) : buffSize(_buffSize), ptr(NULL), root(_root) { rootlen = strlen(root); }
	~fileonDisk() { close(); }
	bool create(const char* name, LARGE_INTEGER size);
	bool create(const char* name, DWORD size) { LARGE_INTEGER li; li.HighPart = 0; li.LowPart = size; return create(name, li); }
	int writeBlock(LARGE_INTEGER pos, char* buff, int len);
	int writeBlock(unsigned int pos, char* buff, int len);
	bool finalise();
	void close();
protected:
	memoryMappedFile mmf;
	int buffSize;
	LARGE_INTEGER fileSize;
	char * ptr; // holds information on block to indicate which ones are full
	int nbBlocks;
	HANDLE h;
	bitVector bv;
	const char* root;
	int rootlen; // rootlen = strlen(root);
	char tmpFileName[512]; // holds name of temporary file which will be delted by finalise() after the reception has completed.
};
