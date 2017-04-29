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

#endif