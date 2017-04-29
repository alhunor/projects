#ifndef _UTILS_H_
#define _UTILS_H_

#include <boost/shared_ptr.hpp>
#include <boost/regex.hpp>
#include <list>
#include <windows.h>

const int MAXLEN=512; // max path length

wchar_t* pointerTo(const wchar_t* const c);

__int64 fileTimeToQuad(FILETIME ft);

bool reloadFileAttributes(const wchar_t* filename, __int64& lastchanged);

char* HexMessage(char* msg, int nbBytes);

template<typename T>
void DeleteIt (T*& ptr)
{
    if (ptr)
    {
        delete[] ptr;
        ptr=NULL;
    }
}

template<typename T>
void dontDelete(T*& ptr)
{
	// will not call delte on the object, useful when object initialised from const
}

boost::shared_ptr<wchar_t> wchar2shared_ptr(wchar_t* wc, void customDeleter(wchar_t*&)=DeleteIt);


class fileInfoT
{
public:
	static const int Added=1;
	static const int Deleted=2;
	static const int Loaded=4;
	static const int Saved=8;
	static const int Removed=16;

	fileInfoT();
	fileInfoT(boost::shared_ptr<wchar_t> _filename, boost::shared_ptr<wchar_t> rpath, __int64 _lastChanged, int _size);
	~fileInfoT()
	{
		size=5;
	}

	boost::shared_ptr<wchar_t> filename;
	boost::shared_ptr<wchar_t> relativepath;
	int size;
	int md5;
	__int64 lastChanged;
	int status;
	bool operator==(fileInfoT& inf);

	struct less
	{
		bool operator()(const fileInfoT* f1, const fileInfoT* f2) const;
	};
};


class matcher
{ // check out http://www.regular-expressions.info/reference.html for regex doc
public:
	matcher() {}
	matcher(wchar_t* filename);
	void init(wchar_t* filename);
	void clear() {fitems.clear(); ditems.clear();}
	~matcher() { clear();}
	bool matchFile(wchar_t* wc) {return match(fitems, wc);}
	bool matchDirectory(wchar_t* wc) {return match(ditems, wc);}
protected:
	bool match(std::list<boost::shared_ptr<boost::wregex> >& items, wchar_t* wc);
	std::list<boost::shared_ptr<boost::wregex> > fitems; //Regex for files
	std::list<boost::shared_ptr<boost::wregex> > ditems; //Regex for directories
};
/*
Symbol	Meaning
c	Match the literal character c once, unless it is one of the special characters.
^	Match the beginning of a line.
.	Match any character that isn't a newline.
$	Match the end of a line.
|	Logical OR between expressions.
()	Group subexpressions.
[]	Define a character class.
*	Match the preceding expression zero or more times.
+	Match the preceding expression one ore more times.
?	Match the preceding expression zero or one time.
{n}	Match the preceding expression n times.
{n,}	Match the preceding expression at least n times.
{n, m}	Match the preceding expression at least n times and at most m times.
\d	Match a digit.
\D	Match a character that is not a digit.
\w	Match an alpha character, including the underscore.
\W	Match a character that is not an alpha character.
\s	Match a whitespace character (any of \t, \n, \r, or \f).
\S	Match a non-whitespace character.
\t	Tab.
\n	Newline.
\r	Carriage return.
\f	Form feed.
\m	Escape m, where m is one of the metacharacters described above: ^, ., $, |, (), [], *, +, ?, \, or /.
*/


#endif