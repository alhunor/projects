// myStream.h: interface for the myStream2 class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _FILESTREAM_H_INCLUDED_
#define _FILESTREAM_H_INCLUDED_

#include <stdlib.h>
#include "stream.h"

class fileStream: public Stream
{
public: 
	fileStream(int _bufferSize) :Stream(_bufferSize) {}
	~fileStream();
	bool open(char* serverName, char* fileName); // <serverName> is ignored
	char get();
protected:
	int fileHandle;
};


#endif // _FILESTREAM_H_INCLUDED_
