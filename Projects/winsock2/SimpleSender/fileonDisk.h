#pragma once

#include <windows.h>
#include "bitlib.h"
#include "myStuff/fileSystems.h"

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
