#include "httpStream.h"
#include <winsock2.h>


httpStream::~httpStream()
{
	WSACloseEvent(e);
	closesocket(S);
	WSACleanup();
}

httpStream::httpStream(int _bufferSize) : Stream(_bufferSize)
{
	WSAStartup (0x202, &W);

}

void httpStream::close()
{
	badStream=true;
	WSAEventSelect(S, e, 0);
	WSACloseEvent(e);
	closesocket(S);
}

bool httpStream::open(char* serverName, char* fileName)
{
	seekindex=0;
	seekpatternlen=0;
	S = socket(AF_INET, SOCK_STREAM,0); // On ne peut rien faire s'il y une erreur.
	if (S==SOCKET_ERROR)
	{
		badStream=true;
		return false;
	};
	e=WSACreateEvent(); //incompatible with WSARecvFrom
	if (e==WSA_INVALID_EVENT)
	{	
		badStream=true;
		return false;
	};
	A.sin_family=AF_INET;
	A.sin_port = htons(80);
	H=gethostbyname(serverName);
	A.sin_addr.s_addr=*((unsigned long *) H->h_addr);
	if (connect(S,(struct sockaddr *) &A,sizeof(A))==SOCKET_ERROR)
	{
		badStream=true;
		return false;
	};
	if (WSAEventSelect(S, e, FD_READ)!=0) //Error
	{
		badStream=true;
		return false;
	};

    char* bb=new char[512];
	strcpy(bb,"GET /");
	strcat(bb,fileName);
	strcat(bb,"\r\n");
	if (send(S,bb,strlen(bb),0)==SOCKET_ERROR)
	{
			badStream=true;
			return false;
	};
	delete[] bb;

	index=0;
	//Receiving the first packet
	switch(::WaitForSingleObject(e,3000))
	{
		case WAIT_OBJECT_0:
			ResetEvent(e);
			bufferedBytes=recv(S,buffer,bufferSize,0); 
			break;
		default:
			badStream=true;
			return false;
	};//switch

	badStream=false;
	return true;
}

char httpStream::get()
{
	if (seekindex<seekpatternlen)
	{
		return seekpattern[seekindex++];
	};
	if (index<bufferedBytes)
		return buffer[index++];
	if (badStream)
		return 0;
	//Need another packet
	switch(::WaitForSingleObject(e,3000)) // The event won't be set when the last packet of size 0 is sent
	{
		case WAIT_OBJECT_0:
			ResetEvent(e);
			bufferedBytes=recv(S,buffer,bufferSize,0); 
			break;
		default:
			badStream=true;
			index=bufferSize;
			return 0;
	};//switch

	if (bufferedBytes<=0)
	{
		badStream=true;
		index=bufferSize;
		return 0;      
	};
	index=1;
	return buffer[0];
}

bool httpStream::seek(char* pattern)
{
	if (seekpattern!=NULL)
	{
		delete[] seekpattern;
		seekpattern=NULL;
		seekpatternlen=0;
	}
	int patternlen=strlen(pattern);
	seekpatternlen=0;
	bool found=false;
	char c=get();
	int searchindex=0;
	while (!found && !badStream)
	{
		if (pattern[searchindex]==c)
		{
			if (searchindex==patternlen-1)
			{
				found=true;
			} else 
			{
				searchindex++;
				c=get();
			}	
		} else
		{
			searchindex=0;
			c=get();
		};
	}
	if (found)
	{
		seekindex=0;
		seekpatternlen=patternlen;
		seekpattern=new char[patternlen+1];
		strcpy(seekpattern,pattern);
	} else
		seekpatternlen=0;
	return found;
}