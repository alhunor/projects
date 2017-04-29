#include "stdAfx.h"
#include "fileStream.h"
#include <io.h>
#include <fcntl.h>


fileStream::~fileStream()
{
	_close(fileHandle);
}

bool fileStream::open(char* serverName, char* fileName)
{
	if ((fileHandle=_open(fileName,_O_RDONLY))==-1)
		return false;
	bufferedBytes=_read(fileHandle,buffer,bufferSize);
	if(bufferedBytes<=0)
		return false;
	badStream=false;
	return true;
}

char fileStream::get()
{
	if (index<bufferedBytes)
		return buffer[index++];
	bufferedBytes=read(fileHandle,buffer,bufferSize);
	if (bufferedBytes<=0)
	{
		badStream=true;
		index=bufferSize;
		return 0;      
	};
	index=1;
	return buffer[0];
}
