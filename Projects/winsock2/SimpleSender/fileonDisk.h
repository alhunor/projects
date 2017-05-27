#pragma once

#include <windows.h>
#include "bitlib.h"

class fileonDisk
{
public:
	fileonDisk(int _buffSize) : buffSize(_buffSize), ptr(NULL) {}
	~fileonDisk() { close(); }
	bool create(const char* name, LARGE_INTEGER size);
	bool writeBlock(LARGE_INTEGER pos, char* buff, int len);
	bool writeBlock(unsigned int pos, char* buff, int len);
	void close();
protected:
	int buffSize;
	LARGE_INTEGER fileSize;
	unsigned char * ptr; // holds information on block to indicate which ones are full
	int nbBlocks;
	HANDLE hFile, hMapFile, h;
	bitVector bv;
};

