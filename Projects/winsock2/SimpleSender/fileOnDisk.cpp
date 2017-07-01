#define _CRT_SECURE_NO_WARNINGS

#include "fileonDisk.h"
#include "myStuff/fileSystems.h"
#include <windows.h>
#include <iostream>


#pragma warning (disable :4244 4334)

void fileonDisk::close()
{
	if (ptr)
	{
		mmf.close();
		CloseHandle(h);
		ptr = NULL;
	}
} // void fileonDisk::close()



bool fileonDisk::create(const char* name, LARGE_INTEGER size)
{
	int len = strlen(name);

	if (len + rootlen > 500)
	{
		std::cout << "File name is too long." << std::endl;
		return false;
	}

	strcpy(tmpFileName, root);
	if (name[1] == ':')
	{ // comes with full path, including drive
		tmpFileName[rootlen] = name[0];
		tmpFileName[rootlen+1] = 0;
		strcat(tmpFileName+rootlen, name+2);
	} else
	{
		if (name[0] == '\\' || name[0] == '/')
		{
			name++;
		}
		strcat(tmpFileName + rootlen, name);
	}

	CreateDirectoryForFile(tmpFileName);
	// create data file
	h = CreateBigFile(tmpFileName, size);
	fileSize = size;

	// create and initialise buffer. The buffer is replicated in a memory mapped file.

	strcat(tmpFileName, ".tmp");
	nbBlocks = size.QuadPart >> (buffSize + 3);
	nbBlocks++;

	// Create memory mapped file holding buffer[nbBlobks]
	mmf.create(tmpFileName, nbBlocks);
	ptr = mmf.getPtr();

/*	hFile = CreateFile(tmpFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); // can't be shared
	if (hFile == INVALID_HANDLE_VALUE)
	{
		std::cout << "Error creating memory mapped file" << std::endl;
		return false;
	}
	hMapFile = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, nbBlocks, NULL);

	ptr = (unsigned char*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, nbBlocks);
*/
	// zero the memory and then init the bitVector
	memset(ptr, 0, nbBlocks);
	bv.init(nbBlocks * 8, ptr);

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
	mmf.close();
	CloseHandle(h);

	int e;
	if (!DeleteFile(tmpFileName))
	{
		e = GetLastError();

	}

	ptr = NULL;
	return true;
}
