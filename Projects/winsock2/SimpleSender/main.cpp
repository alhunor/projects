#define _CRT_SECURE_NO_WARNINGS

//#include <winsock.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <stdlib.h>
#include <conio.h>

#include "bitlib.h"
#include "fileOnDisk.h"
#include "client.h"

#include "WSAUtils.h"



using namespace std;

// link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

const int DEFAULT_BUFLEN = 512;
int nfilecount;

struct fileListType
{
	int nSize;
	unsigned char* pData;
};

fileListType fileList[10];

void function(const char* str)
{
	printf(str);
}
void function(const char* str, int d)
{
	printf(str, d);
}

void function(const char* str, int d, int d2)
{
	printf(str, d, d2);
}

void function(const char* str, int d, int d2, int d3)
{
	printf(str, d, d2, d3);
}

//void RecvThread(Object *lpv)
void RecvThread(SOCKET& clientsocket)
{
	char recvbuf[4096 * 10];
	int nrecvret;
	do
	{
		nrecvret = recv(clientsocket, recvbuf, 4096 * 10, 0);
		if (nrecvret > 0)
		{
			//          function("Bytes received: %d\n", nrecvret);
			BYTE msgID = recvbuf[0];
			switch (msgID)
			{
			case 100:
			{
				nfilecount++;
				DWORD dwFileSize = *(DWORD*)(recvbuf + 1);
				function("Server > File size: %d.\n", dwFileSize);
				fileList[nfilecount].nSize = dwFileSize;
				fileList[nfilecount].pData = (unsigned char*)malloc(dwFileSize);
				break;
			}
			case 101:
			{

				BYTE *pData = (BYTE*)recvbuf;
				pData++;
				unsigned int nSection = *(DWORD*)(pData);
				pData += 4;
				unsigned int nLength = *(DWORD*)(pData);
				pData += 4;

				function("Server > Recieved msg id item data! Section %d, length %d\n", nSection, nLength);
				for (unsigned int i = 0; i < nLength; i++)
					fileList[nfilecount].pData[nSection + i] = pData[i];
				if (nSection + nLength != fileList[nfilecount].nSize)
				{
					/*BYTE* pSendPacketOrg = (BYTE*)malloc(0x5);
					BYTE* pSend = pSendPacketOrg;
					pSend[0] = 200;
					pSend++;
					*(DWORD*)pSend = nSection + nLength;
					function("Server > %d : %d. Requesting %d...\n", nSection, nLength, nSection + nLength);
					send(clientsocket, (char*)pSendPacketOrg, 5, 0);
					*/
				}
				if (nSection + nLength == fileList[nfilecount].nSize)
				{
					function("Server > Recieved all sections!\n");
					BYTE pSend = 201;
					send(clientsocket, (char*)&pSend, 1, 0);
				}
				break;
			}
			case 102:
			{
				function("Server > finished recieving file!\n");
				FILE* file = fopen("FileRecieved.dat", "wb");
				if (file)
				{
					int nWritten = 0;
					while (nWritten < fileList[nfilecount].nSize)
					{
						int nToWrite = 1024*1024; // writing 1 MB chunks
						int Remaining = fileList[nfilecount].nSize - nWritten;
						if (Remaining <= nToWrite)
							nToWrite = Remaining;
						int nBytesWritten = fwrite(fileList[nfilecount].pData + nWritten, 1, nToWrite, file);
						nWritten += nBytesWritten;
					}
				}
				fclose(file);
				break;
			}
			case 't':
			{
				// Echo the buffer back to the sender
				function("Server > Recieved handshake!\n");
				int nsendret = send(clientsocket, recvbuf, nrecvret, 0);
				if (nsendret == SOCKET_ERROR)
				{
					function("Server > send failed with error: %d\n", WSAGetLastError());
					//              closesocket(clientsocket);
					//              WSACleanup();
					break;
				}
				//                  function("Bytes sent: %d\n", nsendret);
				break;
			}
			default:
			{
				function("Server > Unknown msgID: %d, size: %d\n", msgID, nrecvret);
				function("Server > Last error: %d\n", WSAGetLastError());
				break;
			}
			}
		} else if (nrecvret < 0)
		{
			if (errno == EAGAIN)
			{
				function("Server > recv() timed out.\n");
			} else
			{
				function("Server > recv() failed due to errno = %d\n", errno);
			}
		}
		else if (nrecvret == 0)
		{
			function("Server > Connection closing...\n");
		} else if (WSAGetLastError() == 10053)
		{
			function("Server > An established connection was aborted by the software in your host computer, possibly due to a data transmission time-out or protocol error.\n");
			break;
		} else
		{
			function("Server > recv failed with error: %d\n", WSAGetLastError());
			break;
		}

	} while (nrecvret > 0);

	// shutdown the connection since we're done
	function("Server > Shutting down and closing socket %d...\n", clientsocket);
/* Values for the how parameter in shutdown(socket, how) - defined in winsock2
	SD_RECEIVE or 0 Further receives are disallowed
	SD_SEND or 1 Further sends are disallowed
	SD_BOTH or 2 Further sends and receives are disallowed
*/
	nrecvret = shutdown(clientsocket, SD_BOTH);
	if (nrecvret == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		function("Server > shutdown failed with error: %d\n", error);
		function(WSAErrorMessage(error));
		closesocket(clientsocket);
		//      WSACleanup();
	}
	function("Server > Connection closed.");
}

// http://stackoverflow.com/questions/4160347/close-vs-shutdown-socket


int init()
{
	printf("Server > Initialising Winsock...");
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Server > Failed. Error Code : %d", WSAGetLastError());
		return -1;
	}

	printf("Server > Initialised.\n");
	return 0;
}



DWORD WINAPI start(LPVOID lpParam)
{
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		printf("Server > WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	SOCKET s, clientSocket = INVALID_SOCKET;

	struct sockaddr_in server, client;
	int c;

	//init();

	//Create a socket
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Server > Could not create socket : %d", WSAGetLastError());
	}

	printf("Server > Socket created.\n");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(1234);

	//Bind
	if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Server > Bind failed with error code : %d", WSAGetLastError());
	}

	puts("Server > Bind done");

	//Listen to incoming connections
	listen(s, 3);

	//Accept and incoming connection
	puts("Server > Waiting for incoming connections...");

	c = sizeof(struct sockaddr_in);
	clientSocket = accept(s, (struct sockaddr *)&client, &c);
	if (clientSocket == INVALID_SOCKET)
	{
		printf("Server > Accept failed with error code : %d", WSAGetLastError());
	}

	puts("Server > Connection accepted.");

	//Reply to client
	char* message = "bHello Client , I have received your connection.";
	send(clientSocket, message, strlen(message), 0);

	RecvThread(clientSocket);
	getchar();

	closesocket(s);
	closesocket(clientSocket);

	WSACleanup();
	return 0;
} // DWORD WINAPI start(LPVOID lpParam)




int main(int argc, char **argv)
{


	char* pData; // "file" data
	int nSize = 32 * 1000 * 1000; // "file" size, 
	pData = (char*)malloc(nSize); // for now these are allocated instead of reading a file
	for (int i = 0; i < nSize; i++)
	{
		pData[i] = i % 256;
	}

	fileonDisk fod(14); // bufflen = 2^14 (16k)
	LARGE_INTEGER size; size.QuadPart = 32 * 1000 * 1000;
	fod.create("name.dat", size);

	for (int i = 0; i < nSize; i+=16384)
	{
		fod.writeBlock(i, pData, 16384);
	}


	char *sendbuf = "This is a test";

	char recvbuf[DEFAULT_BUFLEN+1]; // default buflen is 512
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;


	// Create server thread
	DWORD  threadID;
	HANDLE h = CreateThread(NULL, 0, start, NULL, 0, &threadID);
	Sleep(1000);

	client cli;
	cli.connect("127.0.0.1", "1234");
	iResult = cli.send(sendbuf, (int)strlen(sendbuf));

	printf("Client > Bytes Sent: %ld\n", iResult);

	// Wait for the server to say hi back
	do
	{
		iResult = cli.receive(recvbuf, recvbuflen);
		if (iResult > 0)
		{
			recvbuf[iResult] = 0;
			printf("Client > Received: %s\n", recvbuf);
			break;
		}
		if (iResult < 0)
		{
			printf("Client > recv failed with error: %d\n", WSAGetLastError());
		}
	} while (iResult > 0); // loop until there is data

	Sleep(2000); // wait 2 seconds and then send our data

	cli.sendMultipleChunks(pData, nSize);

	Sleep(1000);

	// shutdown the connection since no more data will be sent
	cli.shutdown();

	// cleanup
	CloseHandle(h);

	_getch();
	return 0;
} //  main