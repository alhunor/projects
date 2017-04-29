#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include <windows.h>
#include <queue>
#include "client.h"
#include "utils.h"


using namespace std;

bool deleted()
{
	return false;
	// XXX - check in the list of delted files, if any
}

void client::scanFolder( bool recursive)
{ // path is non inclusve of the root. To load the root call it with "".
	HANDLE h;

	wchar_t* filename;
	int nr=1;
	std::queue<boost::shared_ptr<wchar_t> > directories;
	wchar_t pathEx[MAXLEN], cliFile[MAXLEN];
	int rellen = 0;
	wchar_t* aux = new wchar_t[rellen+1];// implement checks for overflow
	aux[0]=0;
	boost::shared_ptr<wchar_t> relpath(aux);

begin:
	if (wcslen(repo->sourceroot.get())+ wcslen(relpath.get())+3>MAXLEN)
		throw "Path is too deep.";

	wcscpy(cliFile, repo->sourceroot.get());
	wcscat(cliFile, relpath.get());
	wcscpy(pathEx, cliFile);
	wcscat(cliFile, L"\\backuper.cli");
	rellen = wcslen(relpath.get());
	wcscat(pathEx, L"\\*");

	matcher.clear();
	matcher.init(cliFile);

	repo->loadDirectory(relpath); // load existing repository
	for (h=FindFirstFile(pathEx, &fileData); FindNextFile(h,&fileData)!=0;)
	{
		//wcout<<fileData.cFileName<<endl;
		bool isDirectory = ((FILE_ATTRIBUTE_DIRECTORY & fileData.dwFileAttributes) == FILE_ATTRIBUTE_DIRECTORY);
		//__int64 modified = max(fileTimeToQuad(fileData.ftLastWriteTime), fileTimeToQuad(fileData.ftCreationTime));
		__int64 lastChanged = fileTimeToQuad(fileData.ftLastWriteTime);

		if (isDirectory)
		{
			if (wcscmp(fileData.cFileName, L"..")==0)
				continue; //skip parent folder
			if (matcher.matchDirectory(fileData.cFileName))
			{
				wcout<<fileData.cFileName<<" has been ignored."<<endl;
				continue;
			}
			wchar_t* dirname = new wchar_t[rellen+wcslen(fileData.cFileName)+1+1];
			wcscpy(dirname, relpath.get());
			wcscat(dirname, L"\\");
			wcscat(dirname,fileData.cFileName);
			directories.push(wchar2shared_ptr(dirname) );
		} else 
		{	// we are adding only files
			if (matcher.matchFile(fileData.cFileName))
			{
				wcout<<fileData.cFileName<<" has been ignored."<<endl;
				continue;
			}

			filename = new wchar_t[wcslen(fileData.cFileName)+1];
			// filename is stored in a shared_ptr inside the fileInfo structure, thus it will be freed when info is freed.
			wcscpy(filename, fileData.cFileName);
			fileInfoT* info = new fileInfoT(wchar2shared_ptr(filename), relpath, lastChanged, nr++);
			if (!deleted())
			{
				info->status=fileInfoT::Added;
			} else
			{
				info->status=fileInfoT::Deleted;
			}
			repo->addFile(info); // resubmit every new file
		}
	}
	FindClose(h);
	// commit the full directory now
	repo->commitDirectory(relpath);
	if (!recursive)
		return;
	// then go recursive in subdirectories
	if (!directories.empty())
	{
		relpath = directories.front();
		directories.pop();
	goto begin;
	}
} // bool client::scanFolder(wchar_t* path, bool recursive)


void client::rebuildRepository(bool recursive)
{
	bool b=repo->getNoCopy();
	repo->setNoCopy(true);
	boost::shared_ptr<wchar_t> aux = repo->sourceroot;
	repo->sourceroot=repo->reporoot;
	scanFolder(recursive);
	repo->setNoCopy(b);
	repo->sourceroot = aux;
}
