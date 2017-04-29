#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <set>

#include <boost/regex.hpp>
#include <boost/mpl/set.hpp>

#include "utils.h"
#include "client.h"
#include "repository.h"

using namespace std;


char* GetErrorString()
{ 
    // Retrieve the system error message for the last-error code
    wchar_t* msgBuf;
    DWORD dw = GetLastError(); 

    int f = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&msgBuf,
        0, NULL );

	char* dest= new char[f];
	size_t n = wcstombs(dest,msgBuf, f);
    LocalFree(msgBuf);
	dest[n]=0; // string finishes in Enter + New line.
	return dest;
}

void f()
{
	wchar_t* cst= L"fsdfsdfsd";
	wchar_t* baba=new wchar_t[20];
	wcscpy(baba,cst);
	boost::shared_ptr<wchar_t> p(baba, DeleteIt<wchar_t>);
	boost::shared_ptr<wchar_t> p2(cst, dontDelete<wchar_t>);
	int a=5;
}



int main(int argc, char** argv)
{
// http://onlamp.com/pub/a/onlamp/2006/04/06/boostregex.html?page=2
// http://www.drdobbs.com/184404797
// http://www.informit.com/articles/article.aspx?p=517609
//	using namespace boost::regex_constants;

	wchar_t sourceroot[MAXLEN];
	wchar_t reporoot[MAXLEN];

	if (argc!=3)
	{
		cout<<"Usage: backuper <SourceDir> <RepoDir>"<<endl;
		return 1;
	}
	int len = strlen(argv[1]);
	if (len+1>=MAXLEN)
		throw ("Maximum path length exceeded.");
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, argv[1], len+1, sourceroot, MAXLEN);

	len = strlen(argv[2]);
	if (len+1>=MAXLEN)
		throw ("Maximum path length exceeded.");
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, argv[2], len+1, reporoot, MAXLEN);


	repository repo;
	repo.setRoot(wchar2shared_ptr(sourceroot, dontDelete), wchar2shared_ptr(reporoot,dontDelete));
	client cli(&repo);

//	repo.cleanRepository(true);
//	cli.rebuildRepository(true);
	cli.scanFolder(true);


	cout<<"Done."<<endl;
	char c;
	std::cin>>c;
	return 0;
}

/*
	ifstream f;
	char mot[64];
	f.open("out2.txt");
	uint32_t hh;

	while (! f.eof())
	{
		f.getline(mot,64);
		hh = hashString(mot);
		cout<<mot <<","<<hh<<endl;
	}

	return 0;
*/
