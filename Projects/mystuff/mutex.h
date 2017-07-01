#ifndef _MyMUTEX_H_
#define _MyMUTEX_H_

#include <windows.h>

class mutex
{
public:
	mutex()
	{
		hMutex = CreateMutex(
			NULL,              // default security attributes
			FALSE,             // initially not owned
			NULL);             // unnamed mutex
	}

	bool get()
	{
		DWORD dwWaitResult = WaitForSingleObject(hMutex, INFINITE);
		return dwWaitResult == WAIT_OBJECT_0;
	}
	void release() { ReleaseMutex(hMutex); }

private:
	HANDLE hMutex;
};





// Adapted from Microsoft ATL classes
class CriticalSection
{
public:
	CriticalSection() throw()
	{
		memset(&m_sec, 0, sizeof(CRITICAL_SECTION));
		Init();
	}

	~CriticalSection()
	{
	}

	HRESULT Lock() throw()
	{
		::EnterCriticalSection(&m_sec);
		return S_OK;
	}

	HRESULT Unlock() throw()
	{
		::LeaveCriticalSection(&m_sec);
		return S_OK;
	}
	HRESULT Init() throw()
	{
		HRESULT hRes = S_OK;
		;
		if (! ::InitializeCriticalSectionEx(&m_sec, 0, 0))
		{
			hRes = HRESULT_FROM_WIN32(GetLastError());
		}

		return hRes;
	}

	HRESULT Term() throw()
	{
		::DeleteCriticalSection(&m_sec);
		return S_OK;
	}
	CRITICAL_SECTION m_sec;
};



#endif