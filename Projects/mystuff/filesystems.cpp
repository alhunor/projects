#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCKAPI_ // define this before including windows.h to make sure the latter excludes "winsock.h"
#include <windows.h>

#include <boost/shared_ptr.hpp>
#include "dates.h"
#include "editDistance.h"
#include "filesystems.h"
#include "mutex.h"
#include "streams.h"

#include <iostream>
#include <queue>
#include <string.H>

char* cc(const char* const tempcchar)
{
	char* aux = new char[strlen(tempcchar) + 1];
	strcpy(aux, tempcchar);
	return aux;
} // char* cc(const char* const tempcchar)



void DOHLCVparser(const char* buff, int nbLine, char tokenID, Descriptor * d)
{
	DOHLCV* data = (DOHLCV*)d->dataPtr;
	switch (tokenID)
	{
		case 0:
			data[nbLine].date = dateFromString(buff);
			break;
		case 1:
			data[nbLine].open = (float)atof(buff);
			break;
		case 2:
			data[nbLine].high = (float)atof(buff);
			break;
		case 3:
			data[nbLine].low = (float)atof(buff);
			break;
		case 4:
			data[nbLine].close = (float)atof(buff);
			break;
		case 5:
			data[nbLine].volume = atoi(buff);
			break;
		default:
			throw "Wrong tokenID";
	} // switch (token)
} // void DOHLCVparser(char* buff, int nbLine, char token, Descriptor * d)



bool openCSVFile(const char* filename, char separator, Descriptor * d, void parser(const char* buff, int nbLine, char tokenID, Descriptor * d))
{
	// if file has more than 512 char per line function call will fail.
	char tmp[512];
	memoryMappedTextFile file;
	char* ptr=file.map(filename);
	if (!ptr)
	{
		return false;
	}
	d->nbLines = file.nbLines;

	// define memory stream to mapped text file data
	binstream bin(file.fileSize(), ptr);

	if (d->hasHeader)
	{
		//read header info;
		bin.getline(tmp,512);
		d->nbLines--;
	}
	d->dataPtr = malloc(d->recordSize * d->nbLines);


	// define memory structure for CSV data
	int i,j;
	for (i=0; i<d->nbLines; ++i)
	{
		for (j=0; j <d->nbFields; ++j)
		{
			bin.getToken(tmp, 512, separator);
			parser(tmp, i, j, d);
		}
	}
	return NULL;
} // openCSVFile


void fileFinder::clear()
{
	char* aux;
	if (initialised)
	{
		//...
		while (!directories.empty())
		{
			aux = directories.front();
			directories.pop();
			delete aux;
		}
		if (h)
			FindClose(h);
	};
	initialised = false;
} // void fileFinder::clear()



bool fileFinder::init(const char* basepath)
{
	clear();
	m_hasFile = false;
	rellen = strlen(basepath)+3;
	relpath = new char[rellen];
	strcpy(relpath, basepath);
	
	return getNextFile(true);
} // bool fileFinder::init(const char* basepath)



bool fileFinder::getNextFile(bool findfirst)
{
	char aux[MAXPATH];
	char* dirname;
	int len;

	if (!findfirst) 
	{
		goto findnextfile;
	}

//findfirstfile
	strcpy(aux,relpath);
	strcat(aux, "\\*");
	h = FindFirstFileA(aux, &fileData);
again:
	if (h==NULL)
		return false;
	if (h==INVALID_HANDLE_VALUE)
	{
        GetLastError();
	}

	bool isDirectory = ((FILE_ATTRIBUTE_DIRECTORY & fileData.dwFileAttributes) == FILE_ATTRIBUTE_DIRECTORY);
	if (isDirectory)
	{
		if (strcmp(fileData.cFileName, "..")==0)
			goto findnextfile; //skip parent folder

		if (strcmp(fileData.cFileName, ".")==0)
			goto findnextfile; //skip itself

		len = rellen+strlen(fileData.cFileName)+1+1;
		dirname = new char[len];
		strcpy(dirname, relpath);
		strcat(dirname, "\\");
		strcat(dirname, fileData.cFileName);
		directories.push(dirname);
	} else 
	{	// we are adding a file
		//strcpy(filename, fileData.cFileName);
		m_hasFile = true;
		return true;
	}
findnextfile:
	if (FindNextFileA(h,&fileData)==0)
	{
		FindClose(h);
		//no more files
		delete relpath;
		if (!directories.empty())
		{
			relpath = directories.front();
			rellen = strlen(relpath);
			directories.pop();
			strcpy(aux,relpath);
			strcat(aux, "\\*");
			h = FindFirstFileA(aux, &fileData);
		} else
		{
			noMoreFiles = true;
			return false;
		}
	}
	goto again;
} // bool fileFinder::getNextFile()



char* fileFinder::nextfile()
{
	if (m_hasFile)
	{
		strcpy(filename, fileData.cFileName);
		strcpy(currentPath, relpath);
	} else
	{
		strcpy(filename, "");
	};
	m_hasFile = false;
	getNextFile(false);
	return filename;
} // char* fileFinder::nextfile()



////////////////////////////////////////////////////////////////////////////////////////////////////////////


dbTable::dbTable(char* _dbName)
{
	dbName = _dbName;
	DWORD dw = GetFileAttributesA(dbName);
	if (dw==INVALID_FILE_ATTRIBUTES)
	{// file does not exists, create it
		dbFile = fopen(dbName,"wb+");
	} else
	{
		//reopen existing file
		dbFile = fopen(dbName,"ab+");
	}
	opened = dbFile!=NULL;
}

dbTable::~dbTable()
{
	if (opened)
		fclose(dbFile);
}

bool dbTable::writeRecord(const void* dataPtr, int len)
{
	if (!opened)
		return false;
	return (len == fwrite(dataPtr,1,len,dbFile));
}

/////////////////////////////////////////////////////////////////////////////////////


void* memoryMappeddbTable::map(const char* filename)
{
	OFSTRUCT  of;
	mapped=false;

	fileHandle = (HANDLE )OpenFile (filename, &of,  writable?OF_READWRITE:OF_READ);
	if (!fileHandle)
	{
		return NULL;
	}
	nFileSize = GetFileSize (fileHandle, NULL);;
	if (nFileSize>512*1024*1024) // 0.5 GB
	{
		CloseHandle(fileHandle);
		throw "File is too big";
	}

	memHandle = CreateFileMapping (fileHandle, NULL, writable?PAGE_READWRITE:PAGE_READONLY, 0, 0, NULL);

	rec = MapViewOfFile (memHandle, writable?FILE_MAP_ALL_ACCESS:FILE_MAP_READ, 0, 0, 0);

	mapped=(rec!=NULL);

	return rec;
} // bool memoryMappeddbTable::map(char* filename)


void memoryMappeddbTable::unmap()
{
	if (mapped)
	{
		UnmapViewOfFile (rec);
		CloseHandle (memHandle);
		CloseHandle (fileHandle);
		mapped = false;
	}
} // void memoryMappeddbTable::unmap()



void memoryMappedTextFile::freeLines()
{
	if (hasLines)
	{
		delete[] lineBuff;
		delete[] linePtr;
	}
} // void memoryMappedTextFile::freeLines()


const char* memoryMappedTextFile::getLine(unsigned int k)
{
// returns the kth line from a text file
	int nr=0;
	
	if (k>nbLines) throw "Bad line number.";
	if (!hasLines)
	{
		linePtr = new char*[nbLines]; // XXX Check for empty file and 1 line
		linePtr[0]=ptr;
		maxLineLen = 0;

		for (DWORD i=1; i<nFileSize; i++)
		{
			if (ptr[i]=='\n') 
			{	
				nr++;
				linePtr[nr]=ptr+i+1;
				if (maxLineLen<linePtr[nr]-linePtr[nr-1])
				{
					maxLineLen = linePtr[nr]-linePtr[nr-1];
				}
			}
		} // for (DWORD i=1; i<nFileSize; i++)
		hasLines=true;
		lineBuff = new char[maxLineLen+1];
	}
	
	int n;
	if (k<nbLines-1) 
	{
		n = linePtr[k+1]-linePtr[k];
	} else
	{
		n = nFileSize - (linePtr[k]-ptr);
	}


	memcpy(lineBuff, linePtr[k], n);
	lineBuff[n] = 0;
	return lineBuff;
} // char* memoryMappedTextFile::getLine(unsigned int k)



char* memoryMappedTextFile::map(const char* filename)
{
	ptr = (char*)memoryMappeddbTable::map(filename);


	// handle unicode
	if (ptr[0]==-17 && ptr[1]==-69 && ptr[2]==-65) // HEXA EF BB BF
	{
		ptr+=3;
	}

// Count the number of lines
    int prevEOL=0;
	_nbLines = 0;
	DWORD i;
	for (i=0; i<nFileSize; i++)
	{
		if (ptr[i]==10)
		{	
			if (prevEOL-i>1)
			{
				_nbLines++;
			}
			// ptr[i]=0; No longer needed.
			prevEOL = i;
		}
	}
	if (i-prevEOL>1) _nbLines++;
	nbLinesCounted = true;

	return (char*)rec;
} // bool memoryMappeddbTable::map(char* filename)


TextFile::TextFile(char* _filename)
{
	int len = strlen(_filename)+1;
	filename = new char[len];
	strcpy_s(filename, len, _filename);

	rec = (char*)mmtf.map(filename);
	if (!rec)
		throw ("Error opening file");
}



int nbLines(const char* filename)
{
	unsigned int nbLines = 0;
    FILE *infile = fopen(filename, "r");
    int ch;

    while (EOF != (ch=getc(infile)))
	{
        if ('\n' == ch) ++nbLines;
	};
    return nbLines;
}



int nbLines2(const char* filename)
{
	unsigned int nbLines = 0;

	HANDLE f = CreateFileA( filename, 
				GENERIC_READ,
				0, // No sharing
				NULL,
				OPEN_EXISTING, // 
				FILE_ATTRIBUTE_NORMAL,
				NULL);

	if (GetLastError()==ERROR_FILE_NOT_FOUND )
	{
		throw "File not found.";
	}
	if (!f) throw "Some error";

	char ptr[32000];

	DWORD nbBytes=1;
	DWORD i, aux, maxlen=0;
	DWORD prevpos;

	aux=0;
	prevpos=0;
	while (nbBytes>0)
	{
		ReadFile(f, ptr, 32000, &nbBytes, NULL);
		for (i=0; i<nbBytes; ++i)
		{
			if (ptr[i]==10) 
			{
				++nbLines;
				if (aux + i-prevpos > maxlen)
					maxlen=aux + i-prevpos;
				prevpos = aux + i;
			}
		}
		aux +=nbBytes;
	} // while (nbBytes>0)
	CloseHandle(f);
	
	// don't loose last line if it does not end in '\n'
	if (aux>prevpos-1) // last line has not been counted
	{
		++nbLines;
		if (aux-prevpos > maxlen)
			maxlen=aux-prevpos;
	}

	return nbLines;
} // int nbLines2(const char* filename)

	
bool ShowOpenFileDialog(char* FileName, int FileNameLength, char* filter)
// Open a dialog for selecting a file and returns true if succeeded with the name of the file in the preallocated buffer <FileName>
{
    OPENFILENAMEA ofn ;

    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = GetActiveWindow(); 
    ofn.lpstrDefExt = 0;
    FileName[0] = '\0';
	ofn.lpstrFile = FileName;
    ofn.nMaxFile = FileNameLength;
    ofn.lpstrFilter = filter; 
    ofn.nFilterIndex = 1;
    char strAux[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, strAux);
	ofn.lpstrInitialDir = strAux;
    ofn.lpstrTitle = LPSTR("Open File");
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ENABLESIZING;

    GetOpenFileNameA(&ofn);

    if (strlen(ofn.lpstrFile) == 0) return false;
    return true;
} // ShowOpenFileDialog



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



bool FindFiles::isValidFolder(bool skipSystemFolders)
{
	bool directory;
	directory = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
	if (!directory) return false;

	// Exclude self and parent
	if (strcmp(fd.cFileName, ".") == 0 || strcmp(fd.cFileName, "..") == 0)
	{
		return false;
	}

    if (fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM && skipSystemFolders)
    {
        return false;
    }

    // exclude Subversion folders
	if (strcmp(fd.cFileName, ".svn") == 0)
	{
		return false;
	}
	return true;
} // bool validDirectory(WIN32_FIND_DATAA& fd)


bool FindFiles::isValidFile()
{
	// any file is valid
	bool isFile = (fd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) != 0;
	return isFile;
}


void FindFiles::SetRoot(const char* root)
{
    strcpy(m_root, root);
	closeSearch();
} // void FindFiles::SetRoot(const char* root)


void FindFiles::storeFolder()
{
	// store the directory
	int len = strlen(m_currentFolder);
	int len2 = strlen(fd.cFileName);
	char* newName = new char[len + strlen(fd.cFileName) + 2];
	strcpy(newName, m_currentFolder);
	strcat(newName + len, fd.cFileName);
	strcat(newName + len + len2, "\\");
	folders.push_back(newName);
}


int FindFiles::openFolder(bool includeSubFolders, bool skipSystemFolders)
{
	char searchPattern[MAX_PATH];
	strcpy(searchPattern, m_currentFolder);
	strcat(searchPattern, "*");
	SetLastError(0);
	m_h = FindFirstFileA(searchPattern, &fd);
	int e = GetLastError();
	if (e != ERROR_SUCCESS)
	{
		FindClose(m_h);
		m_insearch = false;
		return e;
	}

	bool keep_looking = true;
	do
	{
        if (isValidFolder(skipSystemFolders))
		{
            if (includeSubFolders) storeFolder();
			keep_looking = false;
		}
		else if (isValidFile())
		{
			keep_looking = false;
		}
		else
		{
			FindNextFileA(m_h, &fd);
			e = GetLastError();
		}
	} while (keep_looking && e == ERROR_SUCCESS);
	return e;
} //int FindFiles::openFolder(WIN32_FIND_DATAA& fd, bool recursive)


bool FindFiles::FindItem(bool includeSubFolders, bool skipSystemFolders)
{
	if (m_insearch)
	{
		FindNextFileA(m_h, &fd);
		if (GetLastError() == ERROR_SUCCESS)
		{
            if (isValidFolder(skipSystemFolders))
			{
				storeFolder();
			}
			return true;
		}
		else
		{
			FindClose(m_h);
			m_h = 0;
		}
		while (folders.size()>0)
		{
			strcpy(m_currentFolder, folders.front());
			folders.pop_front();
            if (openFolder(includeSubFolders, skipSystemFolders) == 0)
				return true;
		}
		// NO_MORE_FILES;
		m_insearch = false;
		if (m_h) FindClose(m_h);
		return false;
	}
	else
	{
		m_insearch = true;
		strcpy(m_currentFolder, m_root);
        return openFolder(includeSubFolders, skipSystemFolders) == 0;
	}
} // void FindFiles::SetRoot(const char* root)


bool matcher::matched(fileData& fd)
{

    if (!wildCharMatch(filter, fd.cFileName.get())) return false;

    if (fd.fileSize<minSize || fd.fileSize>maxSize) return false;

    if (! (attributes && fd.dwFileAttributes)) return false;

	return true;
}


void matcher::init(const char* _filter, DWORD _attributes, ULONGLONG _minSize, ULONGLONG _maxSize)
{
    delete[] filter;
    int len = strlen(_filter);
    filter = new char[len+1];
    strcpy(filter, _filter);
	initialised = true;
    attributes = _attributes;
    minSize = _minSize;
    maxSize = _maxSize;
}

void FindFiles::findAll(files* f, matcher& m,  bool includeSubFolders, bool skipSystemFolders)
{

    while (FindItem(includeSubFolders, skipSystemFolders))
	{
		fileData fd(AllInfo(), CurrentFolder());
		if (!isFolder() && m.matched(fd))
		{
            f->add(fd);
		}
	}
} // void FindFiles::findAll(files* f)



char* FileTimetoString(FILETIME ft, char* str)
{
	char* months[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

	FileTimeToLocalFileTime(&ft, &ft);
	SYSTEMTIME st;
	FileTimeToSystemTime(&ft, &st);
	//cout << st.wYear << "-" << st.wMonth << "-" << st.wDay << " " << st.wHour << ":" << st.wMinute << ":" << st.wSecond;
	sprintf(str, "%04d-%s-%02d %02d:%02d:%02d", st.wYear, months[st.wMonth - 1], st.wDay, st.wHour, st.wMinute, st.wSecond);
	return str;
}


fileData::fileData(WIN32_FIND_DATAA& fd, const char* _path)
{
	dwFileAttributes = fd.dwFileAttributes;
	ftCreationTime = fd.ftCreationTime;
	ftLastAccessTime = fd.ftLastAccessTime;
	ftLastWriteTime = fd.ftLastWriteTime;
	fileSize = (((ULONGLONG)fd.nFileSizeHigh) << 32) + fd.nFileSizeLow;
	cFileName = SPC(cc(fd.cFileName));
	path = SPC(cc(_path));
	FileTimetoString(fd.ftCreationTime, creationTime);
}

fileData& files::operator[] (int i)
{
	return fd[i];
}

const fileData& files::operator[] (int i) const
{
	return fd[i];
}

void files::clear()
{
	m.get();
	fd.clear();
	n = 0;
	m.release();
}

void files::add(fileData _fd)
{
	m.get();
	fd.push_back(_fd);
	++n;
	m.release();
}



// Creates a file of size <size> and return a handle to it. The file is open.
HANDLE CreateBigFile(const char* name, LARGE_INTEGER size)
{
	HANDLE h = CreateFile(name, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h == INVALID_HANDLE_VALUE)
	{
		std::cout << "Error creating file" << std::endl;
		return INVALID_HANDLE_VALUE;
	}
	SetFilePointerEx(h, size, NULL, FILE_BEGIN);
	SetEndOfFile(h);
	return h;
} // HANDLE CreateBigFile(const char* name, LARGE_INTEGER size)

void CreateDirectoryForFile(const char* path)
{
	char folder[MAX_PATH];
	ZeroMemory(folder, MAX_PATH);
	DWORD err;
	const char *end = strchr(path, '\\');
	if (path[1] == ':')
	{
		end = strchr(++end, '\\');
	}
	while (end != NULL)
	{
		strncpy(folder, path, end - path + 1);

		if (!CreateDirectory(folder, NULL))
		{
			err = GetLastError();
			if (err != ERROR_ALREADY_EXISTS)
			{
				// ignore
			}
		}
		end = strchr(++end, '\\');
	}
} // void CreateDirectoryForFile(const char* path)



bool memoryMappedFile::create(const char* fileName, LARGE_INTEGER size)
{
	// create data file
	CreateDirectoryForFile(fileName);
	hFile = CreateBigFile(fileName, size);

	// map view
	hMapFile = CreateFileMapping(hFile, NULL, PAGE_READWRITE, size.HighPart, size.LowPart, NULL);
	ptr = (char*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0); // map the whole file
	fileSize = size;
	return true;
}


bool memoryMappedFile::open(const char* fileName)
{
	hFile = CreateFile(fileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); // can't be shared;
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	GetFileSizeEx(hFile, &fileSize);

	hMapFile = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 0, NULL);
	ptr = (char*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0); // map the whole file

	return true;
}

void memoryMappedFile::close()
{
	if (!ptr) return;
	UnmapViewOfFile(ptr);
	CloseHandle(hMapFile);
	CloseHandle(hFile);
	ptr = NULL;
}

bool memoryMappedFile::resize(LARGE_INTEGER newSize)
{
	if (!ptr) return false;
	if (!::SetFilePointerEx(hFile, newSize, 0, FILE_BEGIN)) return false;
	if (!::SetEndOfFile(hFile)) return false;

	UnmapViewOfFile(ptr);
	ptr = (char*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0); // map the whole file

	fileSize = newSize;
	return true;
}

bool memoryMappedFile::write(unsigned int pos, const char* buff, int bufflen)
{
	LARGE_INTEGER li;
	li.QuadPart = pos;
	return write(li, buff, bufflen);

}

bool memoryMappedFile::write(LARGE_INTEGER pos, const char* buff, int bufflen)
{
	if (pos.QuadPart + bufflen >= fileSize.QuadPart)
	{
		LARGE_INTEGER newSize;
		newSize.QuadPart = pos.QuadPart * 2;
		if (pos.QuadPart + bufflen >= fileSize.QuadPart)
		{
			newSize.QuadPart = pos.QuadPart + bufflen;
		}
		resize(newSize);
	}
	memcpy(ptr + pos.QuadPart, buff, bufflen);
	return true;
}

