#define _CRT_SECURE_NO_WARNINGS

#include "fileonDisk.h"
#include <windows.h>
#include <iostream>


#pragma warning (disable :4244 4334)

// Creates a file of size <size> and return a handle to it. The file is open.
HANDLE CreateBigFile(const char* name, LARGE_INTEGER size)
{
	HANDLE h = CreateFile(name, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h == INVALID_HANDLE_VALUE)
	{
		std::cout << "Error creating file" << std::endl;
		return INVALID_HANDLE_VALUE;
	}
	SetFilePointerEx(h, size, NULL, FILE_BEGIN);
	SetEndOfFile(h);
	return h;
} // HANDLE CreateBigFile(const char* name, LARGE_INTEGER size)


void fileonDisk::close()
{
	if (ptr)
	{
		CloseHandle(hMapFile);
		CloseHandle(hFile);
		CloseHandle(h);
		ptr = NULL;
	}
} // void fileonDisk::close()



bool fileonDisk::create(const char* name, LARGE_INTEGER size)
{
	int len;

	if (len = strlen(name) > 500)
	{
		std::cout << "File name is too long." << std::endl;
		return false;
	}
	strcpy(tmpFileName, name);
	strcat(tmpFileName, ".tmp");

	// create and initialise buffer
	nbBlocks = size.QuadPart >> (buffSize + 3);
	nbBlocks++;

	// Create memory mapped file holding buffer[nbBlobks]
	hFile = CreateFile(tmpFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); // can't be shared
	if (hFile == INVALID_HANDLE_VALUE)
	{
		std::cout << "Error creating memory mapped file" << std::endl;
		return false;
	}
	hMapFile = CreateFileMapping(hFile, _In_opt_ NULL, PAGE_READWRITE, 0, nbBlocks, NULL);

	ptr = (unsigned char*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, nbBlocks);

	// zero the memory and then init the bitVector
	memset(ptr, 0, nbBlocks);
	bv.init(nbBlocks * 8, ptr);

	// create data file
	h = CreateBigFile(name, size);
	fileSize = size;

	return true;
} // bool fileonDisk::create(const char* name, LARGE_INTEGER size)


int fileonDisk::writeBlock(unsigned int pos, char* buff, int len)
{
	LARGE_INTEGER lpos;
	lpos.LowPart = pos;
	lpos.HighPart = 0;
	return writeBlock(lpos, buff, len);
}

int fileonDisk::writeBlock(LARGE_INTEGER pos, char* buff,  int len)
{
	if (pos.QuadPart + len > fileSize.QuadPart)
	{
		std::cout << "Write exceeds file size." << std::endl;
		return false;
	}
	// move in the file;
	SetFilePointerEx(h, pos, NULL, FILE_BEGIN);
	// write data
	DWORD bytes;

	int res = WriteFile(h, buff, len, &bytes, NULL);

	// calculate address in the buffer[] and set the bit describing the block to 1
	int addr = pos.QuadPart >> buffSize;
	bv.setbit(addr);
	return bytes;
} // bool fileonDisk::writeBlock(LARGE_INTEGER pos, char* buff, int bufflen, int blockID)


bool fileonDisk::finalise()
{
	for (int i = 0; i < nbBlocks; ++i)
	{
		if (!bv.getbit(i)) return false;
	}
	UnmapViewOfFile(ptr);
	CloseHandle(hMapFile);
	CloseHandle(hFile);
	CloseHandle(h);

	int e;
	if (!DeleteFile(tmpFileName))
	{
		e = GetLastError();

	}

	ptr = NULL;
	return true;
}
