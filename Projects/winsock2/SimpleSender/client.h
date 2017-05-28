#pragma once

#include <winsock2.h>

//http://www.binarytides.com/code-tcp-socket-server-winsock/


class client
{
public:
	client();
	bool connect(const char *szServerIP, const char *port);
	int send(const void * const endbuf, int len);
	int sendMultipleChunks(const char* fileName, const char* pData, unsigned int nSize);
	int receive(char *recvbuf, int recvbuflen);
	void shutdown();

protected:
	SOCKET ConnectSocket;
	struct addrinfo *result;
};

