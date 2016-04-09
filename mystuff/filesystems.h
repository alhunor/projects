#ifndef _FILESYSTEM_H_
#define _FILESYSTEM_H_

#include <boost/shared_ptr.hpp>
#include <queue>
#include <list>
#include <windows.h>
#include "MotherOfAllBaseObjects.h"
#include "mutex.h"


typedef boost::shared_ptr<char> SPC;


const int MAXPATH = 512;

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



class binstream
{
public:
	binstream(bool _writable=false, int _buffersize=32000) : opened(false), failbit(true), writable(_writable), buffersize(_buffersize), buffer(NULL) {}
	binstream(const char* filename, bool _writable, int _buffersize=32000);
	binstream(const wchar_t* filename, bool _writable, int _buffersize=32000);
	binstream(int _buffersize, char* dataPtr) :  writable(false), buffer(dataPtr), failbit(false), opened(true), inbuffer(_buffersize), pos(0), fileStream(false) {testforUnicode();}

	~binstream() {close();}
	void write_bool( bool b);
	bool read_bool();
	bool fail() {return failbit;}

	void skip(int len); // advances the cursor by "len" bytes. This data will be ignored.

	char read_byte( ); // 8 bit byte
	void write_byte(char b); // 8 bit byte

	void write_int16( __int16 i);
	__int16 read_int16();
	void write_int( int i);
	int read_int();
	int read_bigendianint(); // reads a big endian int and converst it into little endian before returning it
	void write_int64(__int64 i);
	__int64 read_int64();
	double read_double();
	void write_double(double d);
	void write_wchar( wchar_t* wc);
	wchar_t* read_wchar();
	boost::shared_ptr<wchar_t> read_wchar_sptr( );
	int getline(char* dataPtr, int len);
	bool getToken(char* dest, int len, char separator, bool ignoreWhiteSpaces=false);
	int readblock(void* dataPtr, int len);
	
	// Returns number of characters read by last getline() operation
	// result is undefined if getline has not been called yet
	int gcount() {return bytesread;} 

	void write_char(const char* c);
	char* read_char();
	//void reset() {inbuffer=pos; pos=0;} // 
	bool eof();
	bool eof(int localpos);
	bool open(const wchar_t* filename);
	bool open(const char* filename);

protected:
	int buffersize; // max size of the buffer
	int pos; // current position for read/write
	int inbuffer; // number of characters currently in the buffer
	char* buffer;
	bool fileStream;
	HANDLE f;
	void flush();
	void preload();
	char getchar();
	void testforUnicode();
	void close();

	bool writable; 
	
	int bytesread; // set by getline
	bool failbit;
	bool opened;
	bool unicode;
}; // binstream


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

#endif
