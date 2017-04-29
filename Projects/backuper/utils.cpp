#define _CRT_SECURE_NO_WARNINGS

#include <boost/regex.hpp>
#include <fstream>
#include <hash.h> //3rdParty
#include <string.h>
#include <windows.h>
#include "utils.h"

fileInfoT::fileInfoT(boost::shared_ptr<wchar_t> _filename, boost::shared_ptr<wchar_t> rpath, __int64 _lastChanged, int _size)
	: filename(_filename), relativepath(rpath), size(_size),  lastChanged(_lastChanged)
{
	md5 = hashString((const char*)filename.get(), wcslen(filename.get())*2);
}

boost::shared_ptr<wchar_t> wchar2shared_ptr(wchar_t* wc, void customDeleter(wchar_t*&))
{
	boost::shared_ptr<wchar_t> ptr (wc, customDeleter);
	return ptr;
}

bool reloadFileAttributes(const wchar_t* filename, __int64& lastchanged)
{ // returns false if the file is not found
	WIN32_FIND_DATA fileData;
	HANDLE h=FindFirstFile(filename, &fileData);
	if (h==INVALID_HANDLE_VALUE || GetLastError()==ERROR_FILE_NOT_FOUND)
	{
		FindClose(h);
		return false;
	}
	lastchanged = max(fileTimeToQuad(fileData.ftLastWriteTime), fileTimeToQuad(fileData.ftCreationTime));

	FindClose(h);
	return true;
}


bool fileInfoT::less::operator()(const fileInfoT* f1, const fileInfoT* f2) const
{
	// we need an ordering relation in some sense, it does not need to be the lexicographically correct order
	if (f1->md5!=f2->md5) // comparing hash keys is quicker then comparing strings
		return f1->md5<f2->md5;
	// look at filename if hash keys are identical
	int cmp = wcscmp(f1->filename.get(),f2->filename.get());
	if (cmp!=0)
		return cmp<0;
	return false;
	//look at path for identical filenames
	cmp = wcscmp(f1->relativepath.get(),f2->relativepath.get());
	return cmp<0;
};

bool fileInfoT::operator==(fileInfoT& inf)
{
	// we need an ordering relation in some sense, it does not need to be the lexicographically correct order
	if (md5!=inf.md5) // comparing hash keys is quicker then comparing strings
		return false;
	// look at filename if hash keys are identical
	int cmp = wcscmp(filename.get(),inf.filename.get());
	return (cmp==0);
}

wchar_t* pointerTo(const wchar_t* const c)
{
	int len=wcslen(c)+1;
	wchar_t* aux=new wchar_t[len];
	wcscpy(aux, c);
	return aux;
}

__int64 fileTimeToQuad(FILETIME ft)
{
	ULARGE_INTEGER ula;
	ula.LowPart = ft.dwLowDateTime;
	ula.HighPart = ft.dwHighDateTime;

	return ula.QuadPart;
}

char* HexMessage(char* msg, int nbBytes)
 {   
     static char const* szHexDigits = "0123456789abcdef";
	 static char out[64];
     unsigned int b, pos;
	 pos=0;
	 for (int i=0; i<nbBytes; i++)
     {
         b = msg[i];
		 out[pos++] = szHexDigits[(b >> 4) & 0x0f];
		 out[pos++] = szHexDigits[b & 0x0f];
     }
	 out[pos]=0;
	 return out;
}


matcher::matcher(wchar_t* filename)
{
	init(filename);
	// open file
	// read every line and add them to list
/*	boost::shared_ptr<const wchar_t> egy(L"[-\\w\\.]+obj", noDeleter<const wchar_t>);
	boost::shared_ptr<const wchar_t> ketto(L"[-\\w\\.]+aux", noDeleter<const wchar_t>);
	wchar_t* baba = L"tata";
	boost::shared_ptr<wchar_t> harom(baba, noDeleter<wchar_t>);
	items.push_back(egy);
	items.push_back(ketto);
	items.push_back(harom);*/
}


bool matcher::match(std::list<boost::shared_ptr<boost::wregex> >& items, wchar_t* wc)
{
	std::list<boost::shared_ptr<boost::wregex> >::const_iterator it;
	for (it = items.begin(); it!=items.end(); ++it)
	{
		boost::wregex* reg;
		reg=(*it).get();
		if (boost::regex_match(wc, *reg))
			return true;
	}
	return false;
} // matcher::match(wchar_t* wc)


void matcher::init(wchar_t* filename)
{
	char s[256];
	wchar_t ws[256];
	boost::shared_ptr<boost::wregex> rep;
	boost::wregex* wre;

	std::ifstream in;
	in.open(filename);
	if ( in.fail() || in.bad() )
		return;

	char fileOrDir;
	int len;
	while (!in.eof())
	{
		//First character in every line tells wheter the rule applies to a File(F) or Directory(D)
		in>>fileOrDir;
		in.getline(s,256);
		len = strlen(s);
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, s, len+1, ws, 256);

		try
		{
			// Set up the regular expression for case-insensitivity
			wre=new boost::wregex;
			wre->assign(ws, boost::regex_constants::icase);
			rep.reset(wre);
			if (fileOrDir=='F')
				fitems.push_back(rep);
			if (fileOrDir=='D')
				ditems.push_back(rep);

		} catch (boost::regex_error& e)
		{
			//std::cout << s << " is not a valid regular expression: \"" << e.what() << "\"" << std::endl;
			std::wcout << ws << " is not a valid regular expression: \"" << e.what() << "\"" << std::endl;
		}
	}
} // void matcher::init(wchar_t* filename)
