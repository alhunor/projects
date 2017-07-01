#pragma once

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

class winError
{
public:
	winError() = delete;
	winError(DWORD dw)
	{
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0, NULL);
	}

	LPCTSTR ErrorMessage()
	{
		return (LPCTSTR)lpMsgBuf;
	}

	~winError()
	{
		LocalFree(lpMsgBuf);
	}

protected:
	LPVOID lpMsgBuf;
}; // class winError

void ErrorExit(LPTSTR lpszFunction);