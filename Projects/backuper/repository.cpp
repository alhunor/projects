#define _CRT_SECURE_NO_WARNINGS

#include <boost/shared_ptr.hpp>
#include <fstream>
#include <iostream>
#include <list>
#include <queue>
#include <string.h>
#include <windows.h>
#include "filesystems.h"
#include "repository.h"
#include "utils.h"


using namespace std;
// TODO
// - the length of sourceroot and reporoot is taken several times. While this could be optimised it is unlikely to affect the performence of the application which does a lot of disk access.

bool repository::addFile(fileInfoT* info)
{
	bool found=false;
	set<fileInfoT*, fileInfoT::less>::iterator it;
	it = itemset.find(info);
	if (it==itemset.end())
	{ // new element
		itemset.insert(info);
	} else
	{
		__int64 lastChanged=0;
		wchar_t filename[MAXLEN];

		if ( wcslen(reporoot.get())+wcslen(info->relativepath.get())+1+wcslen(info->filename.get())+1>MAXLEN)
			throw("Maximum path length exceeded.");

		wcscpy(filename, reporoot.get());
		wcscat(filename, info->relativepath.get());
		wcscat(filename, L"\\");
		wcscat(filename, info->filename.get());
		fileInfoT* f = (*it);
		if (f->status == fileInfoT::Loaded)
		{
			//check that the file is still there physically.		
			reloadFileAttributes(filename, lastChanged);
		}
		f->lastChanged = max(lastChanged, f->lastChanged);

		// need to decide wheter a refresh is needed or not and reflect it on the attributes
		if (f->lastChanged < info->lastChanged)
		{
			f->status |= fileInfoT::Added & info->status;
			f->lastChanged = info->lastChanged;
		}
	}

	nbItems++;
	return true;
} // bool repository::addFile(fileInfoT* info)


void repository::listItems(wchar_t* path)
{
	
	wchar_t* s;
	int len = wcslen(path);

	std::set<fileInfoT*, fileInfoT::less>::const_iterator it;
	for (it=itemset.begin(); it!=itemset.end(); ++it)
	{
		s=(*it)->filename.get();
		if (wcsncmp(path, s, len)==0)
			cout<<s<<"  "<<endl;	
	}
}


void repository::LoadInfoFile(boost::shared_ptr<wchar_t> path, boost::shared_ptr<wchar_t> active)
{
	binstream in(path.get(), false);
	int size;
	int md5;
	__int64 lastChanged;
	//boost::shared_ptr<wchar_t> wfilename;
	//wchar_t* wfilename;
	boost::shared_ptr<wchar_t> wfilename;

	while (!in.eof())
	{
		lastChanged=in.read_int64();
		size=in.read_int();
		md5=in.read_int();
		//wfilename=in.read_wchar_sptr();
		boost::shared_ptr<wchar_t> wfilename=in.read_wchar_sptr();
		/*
		char* mbsfilename=in.read_char();
		cout<<nr<<"  "<<mbsfilename<<endl;
		wfilename = new wchar_t[strlen(mbsfilename)+1];
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, mbsfilename, strlen(mbsfilename)+1, wfilename, MAXLEN);
		delete[] mbsfilename;
		*/
		fileInfoT* info=new fileInfoT(wfilename, active, lastChanged, size);
		info->status=fileInfoT::Loaded;
		addFile(info);
	}
} // void repository::LoadInfoFile(wchar_t* path)


bool RepositoryFileIsUptoDate(wchar_t* newFile, fileInfoT* info)
{
	WIN32_FIND_DATA fileData;
	HANDLE h = FindFirstFile(newFile, &fileData);
	if (h==INVALID_HANDLE_VALUE)
	{
		FindClose(h);
		return false;
	}
	FindClose(h);
	// check that file in the repository not older than the one we are trying to copy from the client
	if (info->lastChanged >= max(fileTimeToQuad(fileData.ftLastWriteTime), fileTimeToQuad(fileData.ftCreationTime)))
		return false;
	return true;
//	BY_HANDLE_FILE_INFORMATION fileInfo;
//	GetFileInformationByHandle(h, &fileInfo);
}

void repository::SaveInfoFile(boost::shared_ptr<wchar_t> path)
{
	wchar_t* s;
	fileInfoT* f;
	wchar_t existingFile[MAXLEN], newFile[MAXLEN];

	std::set<fileInfoT*, fileInfoT::less>::const_iterator it;
	
	bool changed=false; //hcs file will be regenerated only if something changes
	for (it=itemset.begin(); it!=itemset.end(); ++it)
	{
		f = (*it);
		s = f->filename.get();
		if (max(wcslen(reporoot.get()), wcslen(sourceroot.get())) + wcslen(f->relativepath.get()) + 1 + wcslen(s) + 1 >MAXLEN)
			throw("Maximum path length exceeded.");
		if ( (f->status & fileInfoT::Deleted) || (f->status & fileInfoT::Added) )
		{	// initialise newfile only if it will be needed later
			wcscpy(newFile, reporoot.get());
			wcscat(newFile, f->relativepath.get());
			wcscat(newFile, L"\\");
			wcscat(newFile, s);
		}
		if (f->status & fileInfoT::Deleted)
		{
			// Delete file from repository
			wcout<<"del "<<newFile<<endl;
			DeleteFile(newFile);
			changed=true;
			continue;
		}
		if (f->status & fileInfoT::Added)
		{
			changed=true;
			if (!RepositoryFileIsUptoDate(newFile, f))
			{
				// Add file to repository
				wcscpy(existingFile, sourceroot.get());
				wcscat(existingFile, f->relativepath.get());
				wcscat(existingFile, L"\\");
				wcscat(existingFile, s);

				if (!noCopy)
				{
					wcout<<"copy "<<existingFile<<" "<<newFile<<endl;
					CopyFile(existingFile, newFile, false);
				}
			}
		}

	} // for (i=itemset.begin(); i!=itemset.end(); ++i)
	if (changed)
	{
		binstream out(path.get(), true);
		for (it=itemset.begin(); it!=itemset.end(); ++it)
		{
			f = (*it);
			// write HCS file
			out.write_int64(f->lastChanged);
			out.write_int(f->size);
			out.write_int(f->md5);
			out.write_wchar(f->filename.get());
			/*char mbs[MAXLEN];
			wcstombs(mbs, f->filename.get(), wcslen(f->filename.get())+1 );
			out.write_char(mbs);*/
		}
	}
} // void repository::SaveInfoFile(wchar_t* path)




void repository::loadDirectory(boost::shared_ptr<wchar_t> active)
{
	itemset.clear();
	// create directory if it does not exists
	wchar_t dirname[MAXLEN];
	if (wcslen(reporoot.get()) + wcslen(active.get()) + 1 + 12 + 1 >MAXLEN)
		throw("Maximum path length exceeded.");

	wcscpy(dirname, reporoot.get());
	wcscat(dirname, active.get());
	
	if (!CreateDirectory(dirname, NULL))
	{
		int err = GetLastError();
		if (err!=ERROR_ALREADY_EXISTS)
			throw "Big ugly error";
	}
	// we have our directory!!
	// load existing files from backuper.hcs, if there is any
	wcscat(dirname, L"\\backuper.hcs");
	try
	{
		LoadInfoFile(wchar2shared_ptr(dirname, dontDelete), active);
	} catch (...) {}
	// XXX should we do more?
}

void repository::commitDirectory(boost::shared_ptr<wchar_t> active)
{
	wchar_t infofile[MAXLEN];

	if (wcslen(reporoot.get())+wcslen(active.get())+12+1+1>MAXLEN) // length of "backuper.hcs" is 12 chars
		throw("Path is too deep.");
	wcscpy(infofile, reporoot.get());
	wcscat(infofile, active.get());
	wcscat(infofile,L"\\backuper.hcs");
	SaveInfoFile(wchar2shared_ptr(infofile, dontDelete) );
}


repository::~repository()
{
	// if itemset has any elements left, go through the structure and free them
	std::set<fileInfoT*, fileInfoT::less>::const_iterator it;
	for (it=itemset.begin(); it!=itemset.end(); ++it)
		delete (*it); // should delete fileInfoT structure
	itemset.clear();
}


ostream& operator<<(ostream& o, repository& rep)
{
	std::set<fileInfoT*, fileInfoT::less>::const_iterator i;
	for (i=rep.itemset.begin(); i!=rep.itemset.end(); ++i)
	{
		o<<(*i)->filename.get()<<"  "<<endl;
	}
	return o;
}



bool repository::cleanRepository(bool recursive)
{
	// will delete all the *.hcs files from the repository
	// path is non inclusve of the root. To load the root call it with "".

	wchar_t filename[MAXLEN];

	HANDLE h;
	WIN32_FIND_DATA fileData;

	std::queue<wchar_t*> directories;
	wchar_t pathEx[MAXLEN];
	int rellen;
	wchar_t* aux = new wchar_t[1];// implement checks for overflow
	aux[0]=0;
	boost::shared_ptr<wchar_t> relpath(aux);

begin:
	if (wcslen(reporoot.get())+ wcslen(relpath.get())+3>MAXLEN)
		throw "Path is too deep.";

	wcscpy(pathEx, reporoot.get());
	wcscat(pathEx, relpath.get());
	rellen = wcslen(relpath.get());
	wcscat(pathEx, L"\\*");

	for (h=FindFirstFile(pathEx, &fileData); FindNextFile(h,&fileData)!=0;)
	{
		//wcout<<fileData.cFileName<<endl;
		bool isDirectory = ((FILE_ATTRIBUTE_DIRECTORY & fileData.dwFileAttributes) == FILE_ATTRIBUTE_DIRECTORY);

		if (isDirectory)
		{
			if (wcscmp(fileData.cFileName, L"..")==0)
				continue; //skip parent folder
			wchar_t* dirname = new wchar_t[rellen+wcslen(fileData.cFileName)+1+1];
			// if all goes well the allocated memory is freed by shared_ptr after it is popped from the list
			// it will leak if code is aborted before pop
			// TODO : convert directories to contain shared ptr's
			wcscpy(dirname, relpath.get());
			wcscat(dirname, L"\\");
			wcscat(dirname,fileData.cFileName);
			directories.push(dirname);
		} else 
		{	// we are adding only files
			if (wcscmp(fileData.cFileName, L"backuper.hcs")==0)
			{
				wcscpy(filename,reporoot.get());
				wcscat(filename,relpath.get());
				wcscat(filename,L"\\backuper.hcs");
				wcout<<filename<<" has been Deleted."<<endl;
				DeleteFile(filename);
				continue;
			}
		}
	}
	FindClose(h);
	if (!recursive)
		return true;
	if (!directories.empty())
	{
		wchar_t* tmp = directories.front();
		relpath.reset(tmp);
		directories.pop();
		goto begin;
	}
	return true;
} // bool repository::cleanRepository(wchar_t* path, bool recursive)