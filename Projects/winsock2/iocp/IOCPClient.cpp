/*

This is a sample code that demonstrates for the following:

* Use of the I/O Completion ports with WinSock.  The idea is to create a simple application that will use IOCP, highlight how to use IOCP. 

* The client will make simple socket calls and server use IOCP to manage them.

Author: Swarajya Pendharkar on 10th March 2006
*/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <winsock2.h>

#include "IOCPClient.h"
#include "IOCPUtils.h"

//Buffer Length 
#define MAX_BUFFER_LEN 256

//Structure to pass information to Thread
struct ThreadInfo
{
	int m_nThreadNo;
	int m_nNoOfSends;
	SOCKET m_Socket;
	char m_szBuffer[MAX_BUFFER_LEN];
};


DWORD WINAPI WorkerThread(LPVOID lpParam)
{
	ThreadInfo *pThreadInfo = (ThreadInfo*)lpParam;

	char szTemp[MAX_BUFFER_LEN];

	int nBytesSent = 0;
	int nBytesRecv = 0;

	for (int ii = 0; ii < pThreadInfo->m_nNoOfSends; ii++)
	{
		sprintf(szTemp, "%d. %s", ii + 1, pThreadInfo->m_szBuffer);

		//Send the message to the server, include the NULL as well
		nBytesSent = send(pThreadInfo->m_Socket, szTemp, strlen(szTemp), 0);

		if (SOCKET_ERROR == nBytesSent)
		{
			WriteToConsole("\nError occurred while writing to socket %ld.", WSAGetLastError());
			return 1; //error
		}

		//Get the message from the server
		nBytesRecv = recv(pThreadInfo->m_Socket, szTemp, 255, 0);

		if (SOCKET_ERROR == nBytesRecv)
		{
			WriteToConsole("\nError occurred while reading from socket %ld.", WSAGetLastError());
			return 1; //error
		}

		//Display the server message
		WriteToConsole("\nServer sent the following message: %s", szTemp);
	}

	return 0; //success
} // DWORD WINAPI WorkerThread(LPVOID lpParam)



bool CreateConnectedSocket(SOCKET *pSocket, char *szHost, int nPortNo)
{
	struct sockaddr_in ServerAddress;
	struct hostent *Server;

	//Create a socket
	*pSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (INVALID_SOCKET == *pSocket)
	{
		WriteToConsole("\nError occurred while opening socket: %d.", WSAGetLastError());
		return false; //error
	}

	//Get the server details
	Server = gethostbyname(szHost);
	//getaddrinfo();

	if (Server == NULL)
	{
		closesocket(*pSocket);
		WriteToConsole("\nError occurred no such host.");
		return false; //error
	}

	//Cleanup and Init with 0 the ServerAddress
	ZeroMemory((char *)&ServerAddress, sizeof(ServerAddress));

	ServerAddress.sin_family = AF_INET;

	//Assign the information received from gethostbyname()
	CopyMemory((char *)&ServerAddress.sin_addr.s_addr, (char *)Server->h_addr, Server->h_length);

	ServerAddress.sin_port = htons(nPortNo);

	//Establish connection with the server
	if (SOCKET_ERROR == connect(*pSocket, (sockaddr*)&ServerAddress, sizeof(ServerAddress)))
	{
		closesocket(*pSocket);
		WriteToConsole("\nError occurred while connecting.");
		return false; //error
	}

	return true;
} // bool CreateConnectedSocket(SOCKET *pSocket, char *szHost, int nPortNo)


int climain(char* hostname, int nPortNo)
{
	//Initialize Winsock
	WSADATA wsaData;

	int nResult = WSAStartup(MAKEWORD(2,2), &wsaData);

	if (NO_ERROR != nResult)
	{
		printf("\nError occurred while executing WSAStartup().");
		return 1; //error
	}

	//Initialize the Console Critical Section
	InitializeCriticalSection(&g_csConsole);


	char szBuffer[MAX_BUFFER_LEN];

	//Read the message from server
	strcpy(szBuffer, "Message to be sent to the server");

	// Number of threads to be created:
	int nNoOfThreads = 12;
	//No. of times we will send the message to the server
	int nNoOfSends = 12;

	HANDLE *p_hThreads = new HANDLE[nNoOfThreads];
	ThreadInfo *pThreadInfo = new ThreadInfo[nNoOfThreads];

	for (int ii = 0; ii < nNoOfThreads; ii++)
	{
		bool bConnectedSocketCreated = CreateConnectedSocket(&(pThreadInfo[ii].m_Socket), hostname, nPortNo);

		if (!bConnectedSocketCreated)
		{
			//Clean up memory
			delete[] p_hThreads;
			delete[] pThreadInfo;

			//failed in creating of connected socket, error out.
			return 1;
		}

		//Populate ThreadInfo
		pThreadInfo[ii].m_nNoOfSends = nNoOfSends;
		pThreadInfo[ii].m_nThreadNo = ii+1;
		sprintf(pThreadInfo[ii].m_szBuffer, "Thread %d - %s", ii+1, szBuffer);

		//Create thread and start banging the server
		DWORD nThreadID;
		p_hThreads[ii] = CreateThread(0, 0, WorkerThread, (void *)(&pThreadInfo[ii]), 0, &nThreadID);
	}

	//Let Worker Threads shutdown
	WaitForMultipleObjects(nNoOfThreads, p_hThreads, TRUE, INFINITE);

	//Delete the Console Critical Section.
	DeleteCriticalSection(&g_csConsole);

	//Close the sockets here
	for (int ii = 0; ii < nNoOfThreads; ii++)
	{
		closesocket(pThreadInfo[ii].m_Socket);
	}

	//Clean up memory
	delete[] p_hThreads;
	delete[] pThreadInfo;

	//Cleanup Winsock
	WSACleanup();
	return 0;
} // int climain(char* hostname, int nPortNo)
