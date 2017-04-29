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
	virtual bool open(char* serverName, char* fileName);
	virtual void close();
	virtual char get();
	virtual bool seek(char* pattern);
	// positionne le courseur en debut de la premiere occurance de <pattern>
	// apres la position actuelle. Si <pattern> n'a pas ete trouve la fonction 
	// renvoie <false>  et le courseur se deplace a la fin du stream.
	// contract: len(pattern)>=2 !!!!!
protected:
	char *server;
	WSAEVENT e;
private:
	bool connected;
	WSADATA W;
	SOCKET S;
	struct sockaddr_in A;
	hostent* H;
// variables for seek
	int seekindex,seekpatternlen;
	char* seekpattern;
};


#endif // _HTTPSTREAM_H_INCLUDED_
