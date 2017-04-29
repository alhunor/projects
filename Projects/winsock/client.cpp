#include "client.h"

#undef UNICODE
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_BUFLEN 512


int client::init()
{
	// Initialize Winsock
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		printf("Client-> WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	destroyed = false;

	return 0;
}

void client::disconnectFromServer()
{
	if (!closed) closesocket(ConnectSocket);

	closed = true;
}

void client::destroy()
{
	if (! destroyed) WSACleanup();
	destroyed = true;
}


int client::connectToServer(const char *servername, const char* port)
{
	int iResult;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(servername, port, &hints, &result);
	if (iResult != 0)
	{
		printf("Client-> getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		destroyed = true;
		return 1;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET)
		{
			printf("Client-> socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			destroyed = true;
			return 1;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			closed = true;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET)
	{
		printf("Client-> Unable to connect to server!\n");
		WSACleanup();
		destroyed = true;
		return 1;
	}

	closed = false;
	destroyed = true;

	return 0;
}




int client::sendFile(const char* path, const char* name)
{
	int pathLen = strnlen_s(path, 512) + 1;
	int nameLen = strnlen_s(name, 512) + 1;
	int headerSize = sizeof(sendHeader) + pathLen + nameLen;
	void* mem = malloc(headerSize);
	sendHeader * sh = (sendHeader *)mem;
	sh->pathStart = 4 * sizeof(int) + 1;
	sh->nameStart = 4 * sizeof(int) + 1 + pathLen;
	sh->md5Start = 4 * sizeof(int) + 1 + pathLen + nameLen;
	strcpy_s(sh->buff, pathLen, path);
	strcpy_s(sh->buff + pathLen, nameLen, name);
	sh->fileSize = 1324;

	// Send an initial buffer
	int iResult = send(ConnectSocket, (const char *) mem, headerSize, 0);
	if (iResult == SOCKET_ERROR)
	{
		printf("Client-> send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		closed = true;
		WSACleanup();
		destroyed = true;
		return 1;
	}

	printf("Client-> Bytes Sent: %ld\n", iResult);

	// shutdown the connection since no more data will be sent
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		printf("Client-> shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		closed = true;
		WSACleanup();
		destroyed = true;
		return 1;
	}

	free(mem);
	return 0;
} // int client::sendFile(const char* path, const char* name)



int client::receive()
{
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	int iResult;

	// Receive until the peer closes the connection
	do
	{

		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
			printf("Client-> Bytes received: %d\n", iResult);
		else if (iResult == 0)
			printf("Client-> Connection closed\n");
		else
			printf("Client-> recv failed with error: %d\n", WSAGetLastError());

	} while (iResult > 0);
	return 0;
}