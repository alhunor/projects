#undef UNICODE
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_PORT "27015"

#include "client.h"
#include "server.h"
#include <strsafe.h>

DWORD WINAPI ServerThread(LPVOID lpParam)
{
	server(DEFAULT_PORT);
	return 0;
}


void ErrorHandler(LPTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code.

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

	// Display the error message.

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	// Free error-handling buffer allocations.

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}




int __cdecl main(int argc, char **argv)
{
/*	DWORD serverThreadID;
	HANDLE serverHandle = CreateThread(
									NULL,                   // default security attributes
									0,                      // use default stack size  
									ServerThread,       // thread function name
									NULL,          // argument to thread function 
									0,                      // use default creation flags 
									&serverThreadID);   // returns the thread identifier 

	if (serverHandle == NULL)
	{
		ErrorHandler(TEXT("CreateThread"));
		ExitProcess(3);
	}

	client cli;
	cli.connectToServer("192.168.1.114", DEFAULT_PORT);
	cli.sendFile("home\\baba", "tata.txt");
	cli.disconnectFromServer();

/*	WaitForSingleObject(serverHandle, INFINITE);
	CloseHandle(serverHandle);
*/
} // int __cdecl main(int argc, char **argv)


