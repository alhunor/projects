#pragma warning (disable : 4996)
#include "streams.h"


bool isWhiteSpace(char c)
{
	switch (c)
	{
		case '\t':
		case '\n':
		case  '\r':
		case ' ': return true;
		default: return false;
	};
}

void binstream::preload() // virtual member implemnted for files
{
	if (!fileStream) return;
	DWORD nbBytes;
	inbuffer -= pos;
	if (inbuffer>0)
	{	// check for overlap - should not be a problem if buffer is much bigger than data items
		memcpy(buffer, &buffer[pos], inbuffer);
	};
	pos = 0;
	ReadFile(f, buffer + inbuffer, buffersize - inbuffer, &nbBytes, NULL);
	inbuffer += nbBytes;
}


bool streamReader::read_bool()
{
	if (pos>inbuffer - 1) preload();
	bool b = *((bool*)&buffer[pos]);
	++pos;
	return b;
}

char streamReader::read_byte()
{
	if (pos>inbuffer - 1) preload();
	return buffer[pos++];
}




bool streamReader::getToken(char* dest, int tokenLen, char separator, bool ignoreWhiteSpaces)
{
	if (isWhiteSpace(separator)) throw "Bad separator.";

	if (tokenLen>inbuffer) preload();
	//	if (tokenLen>inbuffer) throw "tokenLen>StreamSize.";;

	int size = 0, localpos = pos;
	char c;
	while (!eof(localpos) && isWhiteSpace(buffer[localpos]))
	{
		localpos++;
		if (unicode) localpos++;
	}

	while (size<tokenLen && !eof(localpos))
	{
		// getchar
		c = buffer[localpos];
		localpos++;
		if (unicode) localpos++;
		if (c == ' ' || c == '\t' || c == '\r') continue; // skip tabs, spaces and CR

		if (c == separator) break;
		if (c == '\n' || c == 0)
		{
			if (!ignoreWhiteSpaces)
				break;
			continue;
		}
		dest[size] = c;
		size++;
	}
	if (size<tokenLen)
	{
		pos = localpos;
		dest[size] = 0;
		return size>0;
	}
	return false;
} // getToken


__int16 streamReader::read_int16()
{
	if (pos>inbuffer - 2) preload();
	int i = *((__int16*)&buffer[pos]);
	pos += 2;
	return i;
}



int streamReader::read_int()
{
	if (pos>inbuffer - 4) preload();
	int i = *((int*)&buffer[pos]);
	pos += 4;
	return i;
}


int streamReader::read_bigendianint()
{
	if (pos>inbuffer - 4) preload();
	int i = (unsigned char)buffer[pos++];
	i = (i << 8) + (unsigned char)buffer[pos++];
	i = (i << 8) + (unsigned char)buffer[pos++];
	i = (i << 8) + (unsigned char)buffer[pos++];
	return i;
}

double streamReader::read_double()
{
	if (pos>inbuffer - 8) preload();
	double d = *((double*)&buffer[pos]);
	pos += 8;
	return d;
}

__int64 streamReader::read_int64()
{
	if (pos>inbuffer - 8) preload();
	__int64 i64 = *((__int64*)&buffer[pos]);
	pos += 8;
	return i64;
}



wchar_t* streamReader::read_wchar()
{
	wchar_t* wc; //shared ptr would be better
	int len = read_int();
	if (pos>inbuffer - len * 2) preload();
	wc = new wchar_t[len + 1];
	memcpy(wc, &buffer[pos], len * 2);
	pos += len * 2;
	wc[len] = 0;
	return wc;
}

boost::shared_ptr<wchar_t> streamReader::read_wchar_sptr()
{
	wchar_t* wc;
	int len = read_int();
	if (pos>inbuffer - len * 2) preload();
	wc = new wchar_t[len + 1];
	memcpy(wc, &buffer[pos], len * 2);
	pos += len * 2;
	wc[len] = 0;
	boost::shared_ptr<wchar_t> sptr(wc);
	return sptr;
}

char* streamReader::read_char()
{
	char* c; //shared ptr would be better
	int len = read_int();
	if (pos>inbuffer - len) preload();
	c = new char[len + 1];
	memcpy(c, &buffer[pos], len);
	pos += len;
	c[len] = 0;
	return c;
}


bool streamReader::eof()
{
	if (pos<inbuffer)
		return false;

	return true; // end of the memory stream

}

bool binstream::eof()
{
	if (! streamReader::eof())
		return false;

	// read on more data if it is a file stream
	preload();
	return inbuffer == 0;
}

bool streamReader::eof(int localpos)
{
	return (localpos >= inbuffer);
}


char streamReader::getchar()
{
	if (pos>inbuffer || inbuffer == 0) preload();
	char c = buffer[pos];
	pos++;
	if (unicode) pos++;
	return c;
}

int streamReader::getline(char* dataPtr, int len)
// reads characters int dataPtr until a CR/LF or EOF is encountered or len characters have bean read
// returns number of characters read, not including specials
// file pointer is set after CR/LF.
{
	bytesread = 0;
	char c;
	c = getchar();
	while (c != '\n' && c != 0 && bytesread<len && !eof())
	{
		dataPtr[bytesread] = c;
		++bytesread;
		c = getchar();
	}
	dataPtr[bytesread] = 0;

	return bytesread;
} // int binstream::getline(char* dataPtr, int len)



int streamReader::readblock(void* dataPtr, int len)
{
	if (pos>inbuffer - len) preload();
	memcpy(dataPtr, &buffer[pos], len);
	pos += len;
	return len;
}


void streamReader::skip(int len)
{
	//	if (writable) throw myError("Skip work only in read only mode.");
	if (pos>inbuffer - len) preload();
	pos += len;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


binstream::binstream(const wchar_t* filename, bool _writable, int _buffersize) : streamReader(_buffersize, NULL), writable(_writable), failbit(false), opened(false)
{
	open(filename);
}



binstream::binstream(const char* filename, bool _writable, int _buffersize) : streamReader(_buffersize, NULL), writable(_writable), failbit(false), opened(false)
{
	wchar_t aux[512];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, filename, strlen(filename) + 1, aux, 512);
	open(aux);
}


bool binstream::open(const char* filename)
{
	const size_t len = strlen(filename) + 1;
	wchar_t wc[512];
	if (len>512) throw "Filename is too long";
	mbstowcs(wc, filename, len);
	return open(wc);
}


bool binstream::open(const wchar_t* filename)
{
	// http://msdn.microsoft.com/en-us/library/windows/desktop/aa365747(v=vs.85).aspx
	if (opened)
	{
		flush();
		CloseHandle(f);
	}

	inbuffer = 0;
	pos = 0;
	fileStream = true;

	if (buffersize<2)
		throw "Buffer should be at least 2 characters long.";
	if (writable)
	{
		f = CreateFileW(filename,
			GENERIC_WRITE,
			0, // No sharing
			NULL,
			CREATE_ALWAYS, // 
			FILE_ATTRIBUTE_NORMAL,
			NULL);
	}
	else
	{
		f = CreateFileW(filename,
			GENERIC_READ,
			0, // No sharing
			NULL,
			OPEN_EXISTING, // 
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			failbit = true;
			throw "File not found.";
		}
	}
	if (!f)
	{
		failbit = true;
		return false;
	}

	if (!buffer)
		buffer = new char[buffersize];
	preload();

	testforUnicode();

	opened = true;
	return true;
} // binstream::open(wchar_t* filename)


void binstream::testforUnicode()
{
	unicode = ((unsigned char)buffer[0] == 0xFF && (unsigned char)buffer[1] == 0xFE);
	if (unicode)
		pos += 2;
	if (buffer[0] == -17 && buffer[1] == -69 && buffer[2] == -65) // HEXA EF BB BF
	{
		// Skip header for UTF8 encoding
		pos += 3;
	}
}

void binstream::close()
{
	if (!fileStream)
		return;

	if (buffer)
	{
		delete[] buffer;
		buffer = NULL;
	}

	if (!opened)
		return;

	flush();
	CloseHandle(f);
	opened = false;
}


void binstream::flush()
{
	// writes the buffer to the file;
	if (!writable) return;

	if (pos == 0)
		return;
	DWORD NumberOfBytesWritten;
	WriteFile(f, buffer, pos, &NumberOfBytesWritten, NULL);
	pos = 0;
}

void binstream::write_bool(bool b)
{
	if (pos>buffersize - 1) flush();
	*(bool*)&buffer[pos] = b;
	++pos;
}


void binstream::write_byte(char b)
{
	if (pos>buffersize - 1) flush();
	buffer[pos] = b;
	++pos;
}


void binstream::write_int16(__int16 i)
{
	if (pos>buffersize - 2) flush();
	*(__int16*)&buffer[pos] = i;
	pos += 2;
}


void binstream::write_int(int i)
{
	if (pos>buffersize - 4) flush();
	*(int*)&buffer[pos] = i;
	pos += 4;
}


void binstream::write_int64(__int64 i)
{
	if (pos>buffersize - 8) flush();
	*(__int64*)&buffer[pos] = i;
	pos += 8;
}


void binstream::write_double(double d)
{
	if (pos>buffersize - 8) flush();
	*(double*)&buffer[pos] = d;
	pos += 8;
}





void binstream::write_wchar(wchar_t* wc)
{
	int len = wcslen(wc);
	if (pos>buffersize - len * 2 - 4) flush();

	write_int(len);
	memcpy(&buffer[pos], wc, len * 2);
	pos += len * 2;
}


void binstream::write_char(const char* c)
{
	int len = strlen(c);
	if (pos>buffersize - len - 4) flush();

	write_int(len);
	memcpy(&buffer[pos], c, len);
	pos += len;
}

