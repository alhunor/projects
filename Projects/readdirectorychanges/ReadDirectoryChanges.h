//	The MIT License
//	Copyright (c) 2010 James E Beveridge

//	This sample code is for my blog entry titled, "Understanding ReadDirectoryChangesW"
//	http://qualapps.blogspot.com/2010/05/understanding-readdirectorychangesw.html


#pragma once

#include <vector>
#include "ThreadSafeQueue.h"


typedef std::pair<DWORD, std::wstring> TDirectoryChangeNotification;

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

	bool Pop(DWORD& dwAction, std::wstring& wstrFilename);

	// "Push" is for usage by ReadChangesRequest.  Not intended for external usage.
	void Push(DWORD dwAction, std::wstring& wstrFilename);
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
};