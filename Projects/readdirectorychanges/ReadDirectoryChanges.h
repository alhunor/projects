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


#pragma once

#include <atlbase.h>
#include <atlstr.h>
#include <list>
#include <queue>
#include "ThreadSafeQueue.h"


typedef std::pair<DWORD,CString> TDirectoryChangeNotification;
/*struct DirChange
{
	std::wstring fileName;
	DWORD dwAction;
};*/


class CReadChangesServer;

class CReadDirectoryChanges
{
public:
	CReadDirectoryChanges(int nMaxChanges=1000);
	~CReadDirectoryChanges();

	void Init();
	void Terminate();
	bool readConfigFile(const char* filename);

	void AddDirectory(LPCTSTR wszDirectory, BOOL bWatchSubtree, DWORD dwNotifyFilter, DWORD dwBufferSize = 16384);

	// Return a handle for the Win32 Wait... functions that will be signaled when there is a queue entry.
	HANDLE GetWaitHandle() { return m_Notifications.GetWaitHandle(); }

	bool Pop(DWORD& dwAction, CString& wstrFilename);

	// "Push" is for usage by ReadChangesRequest.  Not intended for external usage.
	void Push(DWORD dwAction, CString& wstrFilename);
	bool Excluded(std::wstring& wstrFilename);

	// Check if the queue overflowed. If so, clear it and return true.
	bool CheckOverflow();

	unsigned int GetThreadId() { return m_dwThreadId; }
	bool szWildMatch8(const wchar_t* pat, const wchar_t* str); /// return true if str matches wildchars given by pattern

protected:

	CReadChangesServer* m_pServer;

	HANDLE m_hThread;

	unsigned int m_dwThreadId;

	CThreadSafeQueue<TDirectoryChangeNotification> m_Notifications;

	char mapCaseTable[256]; // used to convert between upercase and lowercase;

	std::vector<std::wstring> exclusions;
//	std::queue<DirChange> notifications;
};