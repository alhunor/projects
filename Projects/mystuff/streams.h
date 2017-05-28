#pragma once

#include <boost/shared_ptr.hpp>
#include <windows.h>


class streamReader
{
public:
	streamReader(int _buffersize = 32000, char* _dataPtr=NULL) : buffersize(_buffersize), buffer(_dataPtr) {}
	virtual ~streamReader() { close(); }
	bool read_bool();
	char read_byte(); // 8 bit byte
	void skip(int len); // advances the cursor by "len" bytes. This data will be ignored.
	__int16 read_int16();
	int read_int();
	int read_bigendianint(); // reads a big endian int and converst it into little endian before returning it
	__int64 read_int64();
	double read_double();
	wchar_t* read_wchar();
	boost::shared_ptr<wchar_t> read_wchar_sptr();
	int getline(char* dataPtr, int len);
	bool getToken(char* dest, int len, char separator, bool ignoreWhiteSpaces = false);
	int readblock(void* dataPtr, int len);

	// Returns number of characters read by last getline() operation
	// result is undefined if getline has not been called yet
	int gcount() { return bytesread; }
	char* read_char();
	virtual bool eof();
	bool eof(int localpos);
	char getchar();


protected:
	char* buffer;
	int buffersize; // max size of the buffer
	int pos; // current position for read/write
	int bytesread; // set by getline
	virtual void close() = 0;
	virtual void preload() = 0;
	int inbuffer; // number of characters currently in the buffer
	bool unicode;

};



class binstream : public streamReader
{
public:
	binstream(bool _writable = false, int _buffersize = 32000) : opened(false), failbit(true), writable(_writable), streamReader(_buffersize, NULL) {}
	binstream(const char* filename, bool _writable, int _buffersize = 32000);
	binstream(const wchar_t* filename, bool _writable, int _buffersize = 32000);
	binstream(int _buffersize, char* dataPtr) : writable(false), streamReader(_buffersize, dataPtr), failbit(false), opened(true), fileStream(false)
	{
		testforUnicode(); 
		inbuffer = _buffersize; 
		pos = 0;
	}

	void write_bool(bool b);
	bool fail() { return failbit; }
	virtual bool eof(); // specific implementation for files

	void write_byte(char b); // 8 bit byte

	void write_int16(__int16 i);
	void write_int(int i);
	void write_int64(__int64 i);
	void write_double(double d);
	void write_wchar(wchar_t* wc);
	void write_char(const char* c);
	//void reset() {inbuffer=pos; pos=0;} // 
	bool open(const wchar_t* filename);
	bool open(const char* filename);

protected:
	bool fileStream;
	HANDLE f;
	void flush();
	void testforUnicode();
	virtual void close();
	virtual void preload();

	bool writable;

	bool failbit;
	bool opened;
}; // binstream


class tcpstreamReader : public streamReader
{
public:

protected:

}; // tcpstreamReader
