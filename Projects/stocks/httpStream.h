// myStream.h: interface for the myStream2 class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _HHTPSTREAM_H_INCLUDED_
#define _HTTPSTREAM_H_INCLUDED_

#include <stdlib.h>
#include <winsock2.h>
#include "stream.h"

class httpStream: public Stream
{
public: 
	httpStream(int _bufferSize);
	~httpStream();
	bool open(char* serverName, char* fileName);
	void close();
	char get();
protected:
	char *server;
	WSAEVENT e;
private:
	bool connected;
	WSADATA W;
	SOCKET S;
	struct sockaddr_in A;
	hostent* H;
};


#endif // _HTTPSTREAM_H_INCLUDED_
