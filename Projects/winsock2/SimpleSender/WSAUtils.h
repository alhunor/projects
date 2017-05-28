#pragma once


const char* WSAErrorMessage(unsigned int id);

#pragma warning (disable : 4200) // dont ant warning for "char fileName [0]" member
struct header
{
	char code;
	DWORD fileSizeLo;
	DWORD fileSizeHi;
	DWORD fileNameLength;
	char fileName[0];
};
#pragma warning (default : 4200)