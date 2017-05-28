#include "client.h"
#include "WSAUtils.h"
#include <winsock2.h>
#include <stdio.h>
#include <ws2tcpip.h>


client::client() : ConnectSocket(INVALID_SOCKET), result(NULL)
{
	WSADATA wsaData;
	// Initialize Winsock
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		printf("Client > WSAStartup failed with error: %d\n", iResult);
	}

}


bool client::connect(const char *szServerIP, const char* port)
{
	// Continue with client, sender
	printf("Client > Starting client");

	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	int iResult = getaddrinfo(szServerIP, port, &hints, &result);
	if (iResult != 0)
	{
		printf("Client > getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return false;
	}
	int count = 0;
	while (1)
	{
		count++;
		printf("Client > [%d] Connecting... \n", count);
		// Create a SOCKET for connecting to server
		ConnectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET)
		{
			printf("Client > socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return false;
		}

		// Connect to server.
		iResult = ::connect(ConnectSocket, result->ai_addr, (int)result->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET)
	{
		printf("Client > Unable to connect to server!\n");
		WSACleanup();
		return false;
	}

	printf("Client > Connected to the server on port 1234!\n");
	return true;
}


int client::send(const void * const sendbuf, int len)
{
	// Send an initial buffer
	int iResult = ::send(ConnectSocket, (char*)sendbuf, len, 0);
	if (iResult == SOCKET_ERROR)
	{
		printf("Client > send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return -1;
	}
	return iResult;
}



int client::receive(char *recvbuf, int recvbuflen)
{
	int iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
	return iResult;
} // int client::receive(char *recvbuf, int recvbuflen)


void client::shutdown()
{
	int iResult = ::shutdown(ConnectSocket, 1);
	if (iResult == SOCKET_ERROR)
	{
		printf("Client > shutdown failed with error: %d\n", WSAGetLastError());
	}
	else printf("Client > shutdown successful.\n");
	closesocket(ConnectSocket);
	WSACleanup();
}


int client::sendMultipleChunks(const char* fileName, const char* pData, unsigned int size)
{
// Send header with <fileName> and <size>
	int len = strlen(fileName); 
	BYTE* pSendPacketOrg = (BYTE*)malloc(1+3*4 + len);
	BYTE* pSendPacket = pSendPacketOrg;
	pSendPacket[0] = 100;
	pSendPacket++;
	*(DWORD*)pSendPacket = size;// lo
	pSendPacket += 4;
	*(DWORD*)pSendPacket = 0;// hi
	pSendPacket += 4;
	*(DWORD*)pSendPacket = len;
	pSendPacket += 4;
	memcpy(pSendPacket, fileName, len);

	printf("Client > Sending 100\n");

	int iResult = send(pSendPacketOrg, 13 + len);
	if (iResult == SOCKET_ERROR)
	{
		printf("Client > send 100 with error: %d\n", WSAGetLastError());
		return -1;
	}
	free(pSendPacketOrg);

// Sending bulk of the file
	unsigned int nWritten = 0;
	printf("Client > Sending 101s\n"); // packet 101 sends the data, in chunks
	unsigned int nToWrite = 1024;
	pSendPacketOrg = (BYTE*)malloc(nToWrite + 1 + 4 + 4);

	while (nWritten < size)
	{
		unsigned int Remaining = size - nWritten;
		//printf("Client > nToWrite %d nRemaining %d\n", nToWrite, Remaining);
		if (Remaining <= nToWrite)
		{
			nToWrite = Remaining;
			//printf("Client > Remaining smaller or equal to write. Writing %d.\n", nToWrite);
		}
		BYTE* pSendPacket = pSendPacketOrg;
		pSendPacket[0] = 101;
		pSendPacket++;
		*(DWORD*)pSendPacket = nWritten;
		pSendPacket += 4;
		*(DWORD*)pSendPacket = nToWrite;
		pSendPacket += 4;
		memcpy(pSendPacket, (pData + nWritten), nToWrite);
		Sleep(100);
		int nBytesWritten = send((char*)pSendPacketOrg, nToWrite + 9);
		if (nBytesWritten == SOCKET_ERROR)
		{
			printf("Client > send 101 : %d:%d with error: %d\n", nWritten, nToWrite, WSAGetLastError());
			return -1;
		}
		///printf("Client > written %d bytes.\n", nBytesWritten);
		nWritten += nBytesWritten - 9; // -9 because of the information before the data
	} // while (nWritten < nSize)
	free(pSendPacketOrg);

// Wait for ACK
	char buff[3];
	if (nWritten == size) // if finished sending everything, wait for a response from the server
	{
		int nResponse = receive(buff, 3);
		if (nResponse == 1)
		{
			if (((BYTE*)buff)[0] == 201) // server notified finished recieving
			{
				printf("Client > Server finished recieving the file...\n");
			}
		}
	}

	printf("Client > Sending 102\n"); // confirming the end of the transmission
	BYTE pEndPacket = 102;
	iResult = send((char*)(&pEndPacket), 1);
	if (iResult == SOCKET_ERROR)
	{
		printf("Client > send 102 with error: %d\n", WSAGetLastError());
		return -1;
	}
	return 1;
} // int client::sendMultipleChunks(const char* pData, int nSize)
