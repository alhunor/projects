#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <windows.h>
#include "repository.h"

class client
{
public:
	client(); // no client without a repository
	client(repository* _repo) {repo=_repo;}
	~client() {}
	void scanFolder(bool recursive);
	void rebuildRepository(bool recursive);

protected:
	repository* repo;
	matcher matcher;
	WIN32_FIND_DATA fileData;
};


#endif