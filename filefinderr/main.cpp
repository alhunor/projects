#include <windows.h>
#include <conio.h>
//#include <WinError.h>
#include <iostream>
#include <list>
#include <boost\shared_ptr.hpp>


#pragma warning (disable : 4996)

using namespace std;

typedef boost::shared_ptr<char> SPC;

char* cc(const char* const tempcchar)
{
	char* aux = new char[strlen(tempcchar)+1];
	strcpy(aux, tempcchar);
	return aux;
} // char* cc(const char* const tempcchar)

class FindFiles
{
public:
	FindFiles();
	void SetRoot(const char* root); // root mus have trailing '\'
	bool FindItem(bool m_recursive=true); // Returns true on succes
	const char* FileName() {return fd.cFileName;}
	WIN32_FIND_DATAA& AllInfo() {return fd;}
	bool isFolder() {return (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) !=0;}
	~FindFiles();
	const char* CurrentFolder() {return m_currentFolder;}

protected:
	void closeSearch();
	int openFolder(bool recursive);
	void storeFolder();
	bool isValidFolder();
	bool isValidFile();
	char m_root[MAX_PATH];
	char m_currentFolder[MAX_PATH];
	bool m_insearch;
	list<char*> folders;
	HANDLE m_h;
	WIN32_FIND_DATAA fd;
}; // FindFiles


struct fileData
{
	fileData();
	fileData(WIN32_FIND_DATAA& fd, const char* _path);
	DWORD dwFileAttributes;
	FILETIME ftCreationTime;
	FILETIME ftLastAccessTime;
	FILETIME ftLastWriteTime;
	DWORD nFileSizeHigh;
	DWORD nFileSizeLow;
	SPC cFileName, path;
};

fileData::fileData(WIN32_FIND_DATAA& fd, const char* _path)
{
	dwFileAttributes = fd.dwFileAttributes;
	ftCreationTime = fd.ftCreationTime;
	ftLastAccessTime = fd.ftLastAccessTime;
	ftLastWriteTime = fd.ftLastWriteTime;
	nFileSizeHigh = fd.nFileSizeHigh;
	nFileSizeLow = fd.nFileSizeLow;
	cFileName = SPC(cc(fd.cFileName));
	path = SPC(cc(_path));
}

class mutex
{
public:
	mutex()
	{
		hMutex = CreateMutex( 
		NULL,              // default security attributes
		FALSE,             // initially not owned
		NULL);             // unnamed mutex
	}

	bool get()
	{
	  DWORD dwWaitResult = WaitForSingleObject(hMutex,INFINITE);
	  return dwWaitResult==WAIT_OBJECT_0;
	}
	void release() {ReleaseMutex(hMutex);}

private:
	HANDLE hMutex;
};


class files
{
public:
	files();
	void clear();
	void add(fileData _fd);

private:
	list<fileData> fd;
	mutex m;
};

void files::clear()
{
	m.get();
	fd.clear();
	m.release();
}

void files::add(fileData _fd)
{
	m.get();
	fd.push_back(_fd);
	m.release();
}

void FindFiles::closeSearch() 
{
	if (m_insearch) CloseHandle(m_h);
	m_insearch = false; 
	folders.clear();
}


FindFiles::FindFiles() : m_insearch(false)
{

}

FindFiles::~FindFiles()
{
	closeSearch();
}



bool FindFiles::isValidFolder()
{
	bool directory;
	directory = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) !=0;
	if (!directory) return false;

	// Exclude self and parent
	if (strcmp(fd.cFileName, ".")==0 || strcmp(fd.cFileName, "..")==0)
	{
		return false;
	}
	// exclude Subversion folders
	if (strcmp(fd.cFileName, ".svn")==0)
	{
		return false;
	}
	return true;
} // bool validDirectory(WIN32_FIND_DATAA& fd)


bool FindFiles::isValidFile()
{
	// any file is valid
	bool isFile = (fd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE ) !=0;
	return isFile;
}


void FindFiles::SetRoot(const char* root)
{
	strcpy(m_root,root);
	closeSearch();
} // void FindFiles::SetRoot(const char* root)


void FindFiles::storeFolder()
{
	// store the directory
	int len = strlen(m_currentFolder);
	int len2 = strlen(fd.cFileName);
	char* newName = new char [ len + strlen(fd.cFileName) + 2];
	strcpy(newName, m_currentFolder);
	strcat(newName + len, fd.cFileName);
	strcat(newName + len + len2, "\\");
	folders.push_back(newName);
}


int FindFiles::openFolder(bool recursive)
{
	char searchPattern[MAX_PATH];
	strcpy(searchPattern, m_currentFolder);
	strcat(searchPattern, "*");
	SetLastError(0);
	m_h = FindFirstFileA(searchPattern, &fd);
	int e = GetLastError();
	if (e!=ERROR_SUCCESS)
	{
		FindClose(m_h);
		m_insearch=false;
		return e;
	}

	bool keep_looking = true;
	do 
	{
		if (isValidFolder())
		{
			if (recursive) storeFolder();
			keep_looking = false;
		} else if (isValidFile())
		{
			keep_looking = false;
		} else
		{
			FindNextFileA(m_h,&fd);
			e = GetLastError();
		}
	} while (keep_looking && e==ERROR_SUCCESS);
	return e;
} //int FindFiles::openFolder(WIN32_FIND_DATAA& fd, bool recursive)


bool FindFiles::FindItem( bool recursive)
{
	if (m_insearch)
	{
		FindNextFileA(m_h,&fd);
		if (GetLastError()==ERROR_SUCCESS)
		{
			if (isValidFolder())
			{
				storeFolder();
			}
			return true;
		} else
		{
			FindClose(m_h);
		}
		while (folders.size()>0)
		{
			strcpy(m_currentFolder, folders.front());
			folders.pop_front();
			if (openFolder(recursive) == 0)
				return true;
		}
		// NO_MORE_FILES;
		m_insearch = false;
		FindClose(m_h);
		return false;
	} else
	{
		m_insearch = true;
		strcpy(m_currentFolder, m_root);
		return openFolder(recursive)==0;
	}
} // void FindFiles::SetRoot(const char* root)


char mapCaseTable[256];
void init_match(VOID)
{
	for (int i = 0; i < 256; ++i)
	{
		mapCaseTable[i] = toupper(i);
	}
}

bool match(const char* pat, const char* str)
// http://xoomer.virgilio.it/acantato/dev/wildcard/wildmatch.html#c_cpp_userjournal_algo
{
	int i, star;

new_segment:

	star = 0;
	if (*pat == '*')
	{
		star = 1;
		do
		{
			pat++;
		} while (*pat == '*');
	}

test_match:

	for (i = 0; pat[i] && (pat[i] != '*'); i++)
	{
		if (mapCaseTable[str[i]] != mapCaseTable[pat[i]])
		{
			if (!str[i]) return 0;
			if ((pat[i] == '?') && (str[i] != '.')) continue;
			if (!star) return 0;
			str++;
			goto test_match;
		}
	}
	if (pat[i] == '*')
	{
		str += i;
		pat += i;
		goto new_segment;
	}
	if (!str[i]) return 1;
	if (i && pat[i - 1] == '*') return 1;
	if (!star) return 0;
	str++;
	goto test_match;
} // bool match(char* pat, char* str)



int main()
{
	init_match();
	cout<<match("*.bab", ".baadhjkd weifbhewoh")<<endl;
	cout<<match("*.bab", "taaa.bab")<<endl;
	cout<<match("*.bab*", "bab.babadhjkd weifbhewoh")<<endl;

//	char* root = "c:\\Project\\";
//	char * filter = "*.etd";
	char * filter = "*.exe";

	FindFiles ff;
	ff.SetRoot("c:\\temp\\");
	files f;
	while (ff.FindItem())
	{
		if ( !ff.isFolder() && match(filter, ff.FileName()))
		{
			cout<<ff.CurrentFolder()<<ff.FileName()<<endl;
			fileData fd(ff.AllInfo(), ff.CurrentFolder());
			f.add(fd);
		}
	}
	cout<<"Terminated. Press any key to exit..."<<endl; getch();
	return 0;
}