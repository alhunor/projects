//	The MIT License
//	Copyright (c) 2010 James E Beveridge



//	This sample code is for my blog entry titled, "Understanding ReadDirectoryChangesW"
//	http://qualapps.blogspot.com/2010/05/understanding-readdirectorychangesw.html
//	See ReadMe.txt for overview information.

#define _CRT_SECURE_NO_DEPRECATE

#include "ReadDirectoryChanges.h"
#include "ReadDirectoryChangesPrivate.h"
#include <fstream>
#include <atlbase.h>

///////////////////////////////////////////////////////////////////////////
// CReadDirectoryChanges

CReadDirectoryChanges::CReadDirectoryChanges(int nMaxCount) : m_Notifications(nMaxCount)
{
	m_hThread	= NULL;
	m_dwThreadId= 0;
	m_pServer	= new CReadChangesServer(this);

	//	Initialise conversion from upercase to lowercase
	for (int i = 0; i < 256; ++i)
	{
		mapCaseTable[i] = tolower(i);
	}
}

CReadDirectoryChanges::~CReadDirectoryChanges()
{
	Terminate();
	delete m_pServer;
}

void CReadDirectoryChanges::Init()
{
	// Kick off the worker thread, which will be managed by CReadChangesServer.

	m_hThread = (HANDLE)_beginthreadex(NULL,
		0,
		CReadChangesServer::ThreadStartProc,
		m_pServer,
		0,
		&m_dwThreadId
		);
}

void CReadDirectoryChanges::Terminate()
{
	if (m_hThread)
	{
		::QueueUserAPC(CReadChangesServer::TerminateProc, m_hThread, (ULONG_PTR)m_pServer);
		::WaitForSingleObjectEx(m_hThread, 10000, true);
		::CloseHandle(m_hThread);

		m_hThread = NULL;
		m_dwThreadId = 0;
	}
}

void CReadDirectoryChanges::AddDirectory( LPCTSTR szDirectory, BOOL bWatchSubtree, DWORD dwNotifyFilter, DWORD dwBufferSize )
{
	if (!m_hThread)
		Init();

	CReadChangesRequest* pRequest = new CReadChangesRequest(m_pServer, szDirectory, bWatchSubtree, dwNotifyFilter, dwBufferSize);
	QueueUserAPC(CReadChangesServer::AddDirectoryProc, m_hThread, (ULONG_PTR)pRequest);
}



void CReadDirectoryChanges::Push(DWORD dwAction, std::wstring& wstrFilename)
{
	m_Notifications.push( TDirectoryChangeNotification(dwAction, wstrFilename) );
}

bool  CReadDirectoryChanges::Pop(DWORD& dwAction, std::wstring& wstrFilename)
{
	TDirectoryChangeNotification pair;
	if (!m_Notifications.pop(pair))
		return false;

	dwAction = pair.first;
	wstrFilename = pair.second;

	return true;
}

bool CReadDirectoryChanges::CheckOverflow()
{
	bool b = m_Notifications.overflow();
	if (b) m_Notifications.clear();
	return b;
}

/*
const wchar_t* towchar(const char *c)
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];
	mbstowcs(wc, c, cSize);
	return wc;
}*/


bool CReadDirectoryChanges::readConfigFile(const char* filename)
{
	typedef enum { Ignore = 0, Settings, Monitor, Exclude} configMode;
	char buff[MAX_PATH + 5];
	wchar_t wc[MAX_PATH + 5];
	configMode mode = Ignore;
	std::fstream in;
	bool recurse;
	in.open(filename);
	const DWORD dwNotificationFlags = FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME;

/*
	FILE_NOTIFY_CHANGE_FILE_NAME (0x00000001) Any file name change in the watched directory or subtree causes a change notification wait operation to return.Changes include renaming, creating, or deleting a file.
	FILE_NOTIFY_CHANGE_DIR_NAME (0x00000002) Any directory - name change in the watched directory or subtree causes a change notification wait operation to return.Changes include creating or deleting a directory.
	FILE_NOTIFY_CHANGE_ATTRIBUTES (0x00000004) Any attribute change in the watched directory or subtree causes a change notification wait operation to return.
	FILE_NOTIFY_CHANGE_SIZE (0x00000008) Any file - size change in the watched directory or subtree causes a change notification wait operation to return.The operating system detects a change in file size only when the file is written to the disk.For operating systems that use extensive caching, detection occurs only when the cache is sufficiently flushed.
	FILE_NOTIFY_CHANGE_LAST_WRITE (0x00000010) Any change to the last write - time of files in the watched directory or subtree causes a change notification wait operation to return.The operating system detects a change to the last write - time only when the file is written to the disk.For operating systems that use extensive caching, detection occurs only when the cache is sufficiently flushed.
	FILE_NOTIFY_CHANGE_LAST_ACCESS (0x00000020) Any change to the last access time of files in the watched directory or subtree causes a change notification wait operation to return.
	FILE_NOTIFY_CHANGE_CREATION (0x00000040) Any change to the creation time of files in the watched directory or subtree causes a change notification wait operation to return.
	FILE_NOTIFY_CHANGE_SECURITY (0x00000100) Any security - descriptor change in the watched directory or subtree causes a change notification wait operation to return.
*/


	int len;
	std::wstring wstr;

	while (!in.eof())
	{
		in.getline(buff, MAX_PATH + 4);
		len = (int)in.gcount();
		if (!buff[0] || buff[0] == '#') continue; // ignore comments and empty lines
		if (buff[0] == '[')  // ignore comments
		{
			if (strcmp(buff, "[Settings]") == 0) mode = Settings;
			else if (strcmp(buff, "[Monitor]") == 0) mode = Monitor;
			else if (strcmp(buff, "[Exclude]") == 0) mode = Exclude;
			else mode = Ignore;
			continue;
		}
		switch (mode)
		{
		case Ignore:
			// Ignore everything outside the Settings, Monitor, Exclude blocks
			break;
		case Exclude:
			mbstowcs(wc, buff + 3, len - 3);
			wstr = wc;
			exclusions.push_back(wstr);
			break;
		case Settings:
			// there are no settings currently
			break;
		case Monitor:
			mbstowcs(wc, buff+3, len-3);
			recurse = (buff[1] == 'r');
			AddDirectory(wc, recurse, dwNotificationFlags);
			break;
		}

	} // while (!in.eof())
	in.close();
	return true;
}


bool CReadDirectoryChanges::szWildMatch8(const wchar_t* pat, const wchar_t* str)
{
	// http://xoomer.virgilio.it/acantato/dev/wildcard/wildmatch.html#c_cpp_userjournal_algo
	// make sure pat is lower case and does not contain consecutive "*"s
	const wchar_t * s, *p;
	bool star = false;

loopStart:
	for (s = str, p = pat; *s; ++s, ++p)
	{
		switch (*p)
		{
		case '?':
			if (*s == '.') goto starCheck;
			break;
		case '*':
			star = true;
			str = s, pat = p;
			if (!*++pat) return true;
			goto loopStart;
		default:
			if (mapCaseTable[*s % 256] != *p) // make sure unicode does not overflow the buffer
				goto starCheck;
			break;
		}
	}
	if (*p == '*') ++p;
	return (!*p);

starCheck:
	if (!star) return false;
	str++;
	goto loopStart;
} // bool szWildMatch8(const char* pat, const char* str)



bool CReadDirectoryChanges::Excluded(std::wstring& wstrFilename)
{
	for (unsigned int i = 0; i < exclusions.size(); ++i)
	{
		if (szWildMatch8(exclusions[i].c_str(), wstrFilename.c_str())) return true;
	}
	return false;
}


