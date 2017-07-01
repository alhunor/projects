#ifndef _FILESYSTEMs_H_
#define _FILESYSTEMs_H_

#include <boost/shared_ptr.hpp>
#include <queue>
#include <list>
#include "mutex.h"
#include <windows.h>
#include "MotherOfAllBaseObjects.h"


typedef boost::shared_ptr<char> SPC;



const int MAXPATH = 512;

// Creates a file of size <size> and return a handle to it. The file is open.
HANDLE CreateBigFile(const char* name, LARGE_INTEGER size);
void CreateDirectoryForFile(const char* path);


bool ShowOpenFileDialog(char* FileName, int FileNameLength, char* filter);

struct Descriptor
{
	int nbLines; // number of liens exluding an optional header line
	int nbFields;// to be filled by caller
	bool hasHeader; // to be filled by caller
	int recordSize; // to be filled by caller
	char** columns;
	void* dataPtr; // allocated in openCSVFile, should be freed by caller
};

struct DOHLCV
{
	int date;
	float open;
	float high;
	float low;
	float close;
	int volume;
};

void DOHLCVparser(const char* buff, int nbLine, char tokenID, Descriptor * d);

bool openCSVFile(const char* filename, char separator, Descriptor * d, void parser(const char* buff, int nbLine, char tokenID, Descriptor * d));


class fileFinder
{
public:
	fileFinder() : initialised(false), noMoreFiles(false) { throw "Use FindFiles class."; }
	~fileFinder() {clear();}
	bool init(const char* basepath); // positions to the first file and returns true or return false if there are no files to be found
	void clear();
	char* nextfile();
	bool eof() {return noMoreFiles;};
	char* getpath() {return currentPath;}
protected:
	bool getNextFile(bool findfirst); // returns true and positions on the next file, or return false if there are no more files
	std::queue<char*> directories;

	bool initialised, noMoreFiles;
	HANDLE h;
	WIN32_FIND_DATAA fileData;
	bool m_hasFile;
	int rellen; // length of current path
	char filename[256];
	char* relpath;
	char currentPath[MAXPATH];
};


struct fileData
{
	fileData() {}
	fileData(WIN32_FIND_DATAA& fd, const char* _path);
	DWORD dwFileAttributes;
	FILETIME ftCreationTime;
	char creationTime[21];
	FILETIME ftLastAccessTime;
	FILETIME ftLastWriteTime;
	SPC cFileName, path;
    ULONGLONG fileSize;
};


class files
{
public:
	files() : n(0) {}
	void clear();
	void add(fileData _fd);
    int nbFiles() const { return n; }
	fileData& operator[] (int i);
	const fileData& operator[] (int i) const;
    void getMutex() { m.get(); }
    void releaseMutex() { m.release(); }

	//	list<fileData>::const_iterator begin() { return fd.begin(); }
	//	list<fileData>::const_iterator end() { return fd.end(); }

private:
	std::vector<fileData> fd;
	mutex m;
	int n; // number of files inserted
};

class matcher
{
public:
    matcher() : initialised(false), filter(NULL) {}
    ~matcher() {if (filter) delete[] filter;}
    void init(const char* _filter, DWORD _attributes, ULONGLONG _minSize, ULONGLONG _maxSize);
	bool matched(fileData& fd);
protected:
	bool initialised;
    DWORD attributes;
    ULONGLONG minSize, maxSize;
    char* filter;
};

class FindFiles
{
public:
	FindFiles();
	void SetRoot(const char* root); // root mus have trailing '\'
    bool FindItem(bool includeSubFolders, bool skipSystemFolders); // Returns true on succes
	const char* FileName() { return fd.cFileName; }
	WIN32_FIND_DATAA& AllInfo() { return fd; }
	bool isFolder() { return (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0; }
	~FindFiles();
	const char* CurrentFolder() { return m_currentFolder; }
    void findAll(files* f, matcher& m, bool includeSubFolders, bool skipSystemFolders);

protected:
	void closeSearch();
    int openFolder(bool includeSubFolders, bool skipSystemFolders);
	void storeFolder();
    bool isValidFolder(bool skipSystemFolders);
	bool isValidFile();
	char m_root[MAX_PATH];
	char m_currentFolder[MAX_PATH];
	bool m_insearch;
	std::list<char*> folders;
	HANDLE m_h;
	WIN32_FIND_DATAA fd;
}; // FindFiles



class dbTable
{
public:
	dbTable(char* _dbName);
	bool init() {return opened;}
	~dbTable();
	bool writeRecord(const void* dataPtr, int len);

protected:
	char* dbName;
	bool opened;
	FILE* dbFile;
};

class memoryMappeddbTable
{
public:
	memoryMappeddbTable(bool _writable=true) : mapped(false), writable(_writable) {}
	~memoryMappeddbTable() { unmap();}
	void* map(const char* filename);
	void unmap();
	void flush() {if (mapped && writable) FlushViewOfFile (memHandle, nFileSize);}
	int fileSize() {return nFileSize;}

protected:
	DWORD nFileSize;
	bool mapped, writable;
    HANDLE fileHandle;
	HANDLE memHandle;
	void* rec;
};


class memoryMappedTextFile : public memoryMappeddbTable
{ // not thread safe
// http://www.cplusplus.com/forum/articles/36872/
private:
	unsigned int _nbLines;

public:
	memoryMappedTextFile() : nbLinesCounted(false), hasLines(false), _nbLines(0), memoryMappeddbTable(false), nbLines(_nbLines) {}
	~memoryMappedTextFile() { freeLines(); }
	char* map(const char* filename);

	const unsigned int& nbLines;

	const char* getLine(unsigned int i);
	void freeLines();

protected:
	bool nbLinesCounted, hasLines;
	char* ptr;
	char** linePtr;
	char* lineBuff; // getLine(int k) returns a const pointer to this structure after it has copied the kth line into it.
	int maxLineLen; // length of the longest line
};



class TextFile : public MotherOfAllBaseObjects
{
public:
	TextFile(char* Filename);
	~TextFile(){}

	TextFile(); // do not implement this

	//xlw::XlfOper list(int minPos, int maxPos, int step);
	const char* name() {return "TextFile";}
	int nbLines() {return mmtf.nbLines; }
	int fileSize() {return mmtf.fileSize(); }
	const char* getLine(int i) {return mmtf.getLine(i);}
	char* filename;

protected:
	memoryMappedTextFile mmtf;
	char* rec;
};


// Return the number of lines for a text file. 
// XXX measure the spead.
int nbLines(const char* filename);
int nbLines2(const char* filename);


class memoryMappedFile
{
public:
	memoryMappedFile() : ptr(NULL) {}
	~memoryMappedFile() { close(); }

	bool create(const char* filename, LARGE_INTEGER size);
	bool create(const char* filename, int size)
	{
		LARGE_INTEGER li;
		li.QuadPart = size;
		return create(filename, li);
	};
	bool open(const char* filename);
	void close();
	bool write(LARGE_INTEGER pos, const char* buff, int bufflen); // resizes file if required
	bool write(unsigned int pos, const char* buff, int bufflen);

	int getlastError() { return GetLastError(); }
	char* getPtr() { return ptr; }
	LARGE_INTEGER getSize() { return fileSize; }

protected:
	bool resize(LARGE_INTEGER newSize);
	HANDLE hFile, hMapFile;
	char* ptr;
	LARGE_INTEGER fileSize;
};



#endif
