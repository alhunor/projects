#ifndef _CLINET_H_H
#define _CLINET_H_H

#include <winsock2.h>

#pragma warning ( disable : 4200)

struct sendHeader
{
	int pathStart;
	int nameStart;
	int md5Start;
	int fileSize;
	char buff[0]; // contains 
};

#pragma warning ( default : 4200)


class client
{
public:
	client() : closed(true), destroyed(true) { init(); }
	~client() { disconnectFromServer(); destroy(); }
	int connectToServer(const char *servername, const char* port);
	int sendFile(const char* path, const char* name);
	void disconnectFromServer();

protected:
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL, *ptr = NULL, hints;
	bool closed, destroyed;
	int init();
	int receive();
	void destroy();
};

#endif
