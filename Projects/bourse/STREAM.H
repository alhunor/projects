// Stream.h: interface for the Stream class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _STREAM_H_INCLUDED_
#define _STREAM_H_INCLUDED_

#include <stdlib.h>
#include <iostream.h>

class Stream  
{
public:
	Stream(int _bufferSize) : index(0),bufferedBytes(0),bufferSize(_bufferSize), badStream(true) { buffer= new char[bufferSize];}
	~Stream() {delete[] buffer;}
	virtual bool open(char* serverName, char* fileName)=0;
	virtual char get()=0;
	virtual bool bad() {return badStream;}
	virtual bool eof() {return badStream;}
	virtual void close() {}
protected:
	int index;
	const int bufferSize;
	int bufferedBytes;
	bool badStream;
	char* buffer;

};

#endif //_STREAM_H_INCLUDED_
