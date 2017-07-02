#define _CRT_SECURE_NO_WARNINGS

#include <conio.h>
#include <iostream>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#include "bitlib.h"
#include "client.h"
#include "fileOnDisk.h"

#include "myStuff/streams.h"

#include "WSAUtils.h"



using namespace std;

// link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

const int DEFAULT_BUFLEN = 512;
int nfilecount;

struct fileListType
{
	DWORD fileSizeLo, fileSizeHi;
	char* fileName;
} fileInfo;


void RecvThread(SOCKET clientsocket)
{
	char rcvBuff[20000], buffer[2048];
	tcpStreamReader tcp(clientsocket, 20000, rcvBuff);
	unsigned int nSection;
	unsigned int nLength;
//	FILE* file;


	fileonDisk fod(10, "L:\\temp\\"); // bufflen = 2^10 (1k)

	do
	{
		BYTE msgID = tcp.read_byte();
		switch (msgID)
			{
			case 100:
			// format is : fileSizeLo(int), fileSizeHi(int), filenameLen(int), filename(char[filenameLen(int)] without terminal NULL)
				nfilecount++;
				fileInfo.fileSizeLo = tcp.read_int();
				fileInfo.fileSizeHi = tcp.read_int();
				fileInfo.fileName = tcp.read_char();
				fod.create(fileInfo.fileName, fileInfo.fileSizeLo);

				printf("Server > Receiving %s, size: %d.\n", fileInfo.fileName, fileInfo.fileSizeLo);
				break;
			case 101:
			// // format is : position(int), len(int), data(char[len] )
				nSection = tcp.read_int();
				nLength = tcp.read_int();

				tcp.readblock(buffer, nLength);
				fod.writeBlock(nSection, buffer, nLength);
				if (nSection + nLength == fileInfo.fileSizeLo)
				{
					printf("Server > Recieved all sections!\n");
					BYTE pSend = 201;
					send(clientsocket, (char*)&pSend, 1, 0);
					fod.finalise();
				}
				break;
			case 102:
				printf("Server > Client acknowledged recption of the file!\n");
				break;
			default:
				printf("Server > Unknown msgID: %d\n", msgID);
				printf("Server > Last error: %d\n", WSAGetLastError());
				break;
		}  //switch(msgID)
/*

*/
	} while (!tcp.eof());

	// shutdown the connection since we're done
	printf("Server > Shutting down and closing socket %d...\n", clientsocket);
/* Values for the how parameter in shutdown(socket, how) - defined in winsock2
	SD_RECEIVE or 0 Further receives are disallowed
	SD_SEND or 1 Further sends are disallowed
	SD_BOTH or 2 Further sends and receives are disallowed
*/
	int nrecvret = shutdown(clientsocket, SD_BOTH);
	if (nrecvret == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		printf("Server > shutdown failed with error: %d\n", error);
		printf(WSAErrorMessage(error));
	}
	printf("Server > Connection closed.");

	printf("Server > Received a total of %d files.", nfilecount);
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
	if ((s = ::socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Server > Could not create socket : %d", WSAGetLastError());
	}

	printf("Server > Socket created.\n");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(1234);

	//Bind
	if (::bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Server > Bind failed with error code : %d", WSAGetLastError());
	}

	puts("Server > Bind done");

	//Listen to incoming connections
	listen(s, 3);

	//Accept and incoming connection
	puts("Server > Waiting for incoming connections...");

	c = sizeof(struct sockaddr_in);
	clientSocket = ::accept(s, (struct sockaddr *)&client, &c);
	if (clientSocket == INVALID_SOCKET)
	{
		printf("Server > Accept failed with error code : %d", WSAGetLastError());
	}

	puts("Server > Connection accepted.");

	RecvThread(clientSocket);

	::closesocket(s);
	closesocket(clientSocket);

	WSACleanup();
	return 0;
} // DWORD WINAPI start(LPVOID lpParam)


#include "blockQueue.h"

int main(int argc, char **argv)
{
///	jb();
/*
	memoryMappedFile mmf;
	if (!mmf.open("L:\\readme.txt"))
	{
		int errorCode = mmf.getlastError();
		cout << "Error = " << errorCode <<endl;
		return errorCode;
	}
	LARGE_INTEGER li = mmf.getSize();
	int size = li.LowPart ;
	cout << "Size = " << size <<endl;
	
	const char* buff = "This has been added recently";
	int bufflen = strlen(buff);
	mmf.write(80, buff, bufflen);

	char*p = mmf.getPtr();
	for (int i = 0; i < size; ++i)
	{
		cout << p[i];
	}
	memcpy(p+90, buff,bufflen);
	mmf.close();
	*/
	bool b = blockQueueTest();

	
	char* pData; // "file" data
 	int nSize = 32 * 1000 * 1000; // "file" size, 
	nSize = 5000; // "file" size, 
	pData = (char*)malloc(nSize); // for now these are allocated instead of reading a file
	for (int i = 0; i < nSize; i++)
	{
		pData[i] = i % 256;
	}

	// Create server thread
	DWORD  threadID;
	HANDLE thread = CreateThread(NULL, 0, start, NULL, 0, &threadID);
	Sleep(1000);

	client cli;
	cli.connect("127.0.0.1", "1234");

	cli.sendMultipleChunks("L:\\sent.dat", pData, nSize);
	cli.sendMultipleChunks("tata\\sent3.dat", pData, nSize);
	cli.sendMultipleChunks("sent5.dat", pData, nSize);

	// shutdown the connection since no more data will be sent
	cli.shutdown();
	// cleanup
	CloseHandle(thread);
	cout << "Press any key (euhm, ENTER) to exit" << endl<<flush;
	_getch();
	return 0;
} //  main