//
//	The MIT License
//
//	Copyright (c) 2010 James E Beveridge
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files (the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions:
//
//	The above copyright notice and this permission notice shall be included in
//	all copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//	THE SOFTWARE.


//	This sample code is for my blog entry titled, "Understanding ReadDirectoryChangesW"
//	http://qualapps.blogspot.com/2010/05/understanding-readdirectorychangesw.html
//	See ReadMe.txt for overview information.

#define _CRT_SECURE_NO_DEPRECATE

//#include "stdafx.h"
#include <iostream>
#include <stdio.h>
#include <Strsafe.h>
#include "ReadDirectoryChanges.h"


LPCWSTR ExplainAction( DWORD dwAction );
bool TryGetKeyboardInput( HANDLE hStdIn, bool &bTerminate, wchar_t* buf );


//
// When the application starts, it immediately starts monitoring your home
// directory, including children, as well as C:\, not including children.
// The application exits when you hit Esc.
// You can add a directory to the monitoring list by typing the directory
// name and hitting Enter. Notifications will pause while you type.



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
	LPCTSTR ErrorMessage() { return (LPCTSTR)lpMsgBuf; }

	~winError()
	{
		LocalFree(lpMsgBuf);
	}

protected:
	LPVOID lpMsgBuf;
}; // class winError


void ErrorExit(LPTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf, LocalSize(lpDisplayBuf) / sizeof(TCHAR), 	TEXT("%s failed with error %d: %s"), lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	//ExitProcess(dw);
} // void ErrorExit(LPTSTR lpszFunction)

void main()
{
	//const DWORD dwNotificationFlags = FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_FILE_NAME;

	// Create the monitor and add two directories.
	CReadDirectoryChanges changes;

	//bool b = changes.szWildMatch8(L"tata*", L"tataba");
	changes.readConfigFile("config.txt");

	HANDLE hStdIn =  ::GetStdHandle(STD_INPUT_HANDLE);
	const HANDLE handles[] = { hStdIn, changes.GetWaitHandle() };

	wchar_t buf[MAX_PATH];
	bool bTerminate = false;
	DWORD dwAction;
	std::wstring wstrFilename;

	while (!bTerminate)
	{
		DWORD rc = ::WaitForMultipleObjectsEx(_countof(handles), handles, false, INFINITE, true);
		switch (rc)
		{
		case WAIT_OBJECT_0 + 0:
			// hStdIn was signaled. This can happen due to mouse input, focus change,
			// Shift keys, and more.  Delegate to TryGetKeyboardInput().
			// TryGetKeyboardInput sets bTerminate to true if the user hits Esc.
			if (TryGetKeyboardInput(hStdIn, bTerminate, buf))
			{
				changes.AddDirectory(buf, false, FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME);
			}
			break;
		case WAIT_OBJECT_0 + 1:
			// We've received a notification in the queue.
			if (changes.CheckOverflow())
			{
				wprintf(L"Queue overflowed.\n");
			} else
			{
				changes.Pop(dwAction, wstrFilename);
				if (changes.Excluded(wstrFilename))
				{
					wprintf(L"Excluded %s: \n", wstrFilename.c_str());
				}
				wprintf(L"%s %s\n", ExplainAction(dwAction), wstrFilename.c_str());

				WCHAR baba[500];
				DWORD res;
				res = GetLongPathNameW(wstrFilename.c_str(), baba, 500);
				if (res == 0)
				{
					res = GetLastError();
					winError w(res);
					wprintf(L"%s\n", w.ErrorMessage());

					//ErrorExit(L"GetLongPathNameW");
					//res = GetLastError();
				} else
				{
					wprintf(L"Long : %s\n", baba);
				}
				res = GetShortPathNameW(wstrFilename.c_str(), baba, 500);
				if (res == 0)
				{
					res = GetLastError();
					winError w(res);
					wprintf(L"%s\n", w.ErrorMessage());

					//ErrorExit(L"GetShortPathNameW");
				} else
				{
					wprintf(L"Short : %s\n", baba);
				}
			} // else branch of if (changes.CheckOverflow())
			break;
		case WAIT_IO_COMPLETION:
			// Nothing to do.
			break;
		}
	}

	// Just for sample purposes. The destructor will
	// call Terminate() automatically.
	changes.Terminate();
}

LPCWSTR ExplainAction( DWORD dwAction )
{
	switch (dwAction)
	{
	case FILE_ACTION_ADDED            :
		return L"Added";
	case FILE_ACTION_REMOVED          :
		return L"Deleted";
	case FILE_ACTION_MODIFIED         :
		return L"Modified";
	case FILE_ACTION_RENAMED_OLD_NAME :
		return L"Renamed From";
	case FILE_ACTION_RENAMED_NEW_NAME :
		return L"Renamed To";
	default:
		return L"BAD DATA";
	}
}

bool TryGetKeyboardInput( HANDLE hStdIn, bool &bTerminate, wchar_t* buf )
{
	DWORD dwNumberOfEventsRead=0;
	INPUT_RECORD rec = {0};

	if (!::PeekConsoleInput(hStdIn, &rec, 1, &dwNumberOfEventsRead))
		return false;

	if (rec.EventType == KEY_EVENT)
	{
		if (rec.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE)
			bTerminate = true;
		else if (rec.Event.KeyEvent.wVirtualKeyCode > VK_HELP)
		{
			if (!fgetws(buf, MAX_PATH, stdin))	// End of file, usually Ctrl-Z
				bTerminate = true;
			else
				return true;
		}
	}

	::FlushConsoleInputBuffer(hStdIn);

	return false;
}