#include <windows.h>
#include <stdio.h>

// https://www.codeproject.com/Articles/13382/A-simple-application-using-I-O-Completion-Ports-an

#include "IOCPUtils.h"

CRITICAL_SECTION g_csConsole; //When threads write to console we need mutual exclusion

int GetNoOfProcessors()
{
	static int nProcessors = 0;

	if (nProcessors == 0)
	{
		SYSTEM_INFO si;

		GetSystemInfo(&si);

		nProcessors = si.dwNumberOfProcessors;
	}
	return nProcessors;
} // int GetNoOfProcessors()



  //Function to synchronize console output
  //Threads need to be synchronized while they write to console.
  //WriteConsole() API can be used, it is thread-safe, I think.
  //I have created my own function.
void WriteToConsole(char *szFormat, ...)
{
	EnterCriticalSection(&g_csConsole);

	va_list args;
	va_start(args, szFormat);

	vprintf(szFormat, args);

	va_end(args);

	LeaveCriticalSection(&g_csConsole);
}