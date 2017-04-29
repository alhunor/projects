#pragma warning (disable : 4090)
#pragma warning (disable : 4996)

#include <io.h>     // For access().
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>  // For stat().
#include <sys/stat.h>   // For stat().
#include<windows.h>

#include "utils.h"




int DirectoryExists( const char* absolutePath )
{

    if( _access( absolutePath, 0 ) == 0 ){

        struct stat status;
        stat( absolutePath, &status );

        return (status.st_mode & S_IFDIR) != 0;
    }
    return 0;
}


int whitespace(char c)
{
	if (c==' ' || c=='\t' || c=='\r' || c=='\n')
		return 1;
	return 0;
}

char* newString(const char* const c)
{	// clonbes an existing string
	char* aux;
	int len = strlen(c)+1;
	aux = (char*) malloc(sizeof(char)*len);
	memcpy(aux, c, len);
	return aux;
}


// allocates new memory and returns a string in which %20s have been converted into spaces
char* normaliseUrlString(const char* const c, int len)
{
	int i, pos;
	char* aux;
	aux = (char*) malloc(sizeof(char)*len);
	pos=0;
	for (i=0; i<len-2; i++)
	{
		if (c[i]=='%' && c[i+1]=='2' && c[i+2]=='0')
		{
			aux[pos]=' ';
			i+=2;
		} else aux[pos]=c[i];
		pos++;
	}
	aux[pos]=0;
	return aux;	
}

char* fileNameFromUrl(const char* const url, int len)
{
	int i = len;
	while (i>0 && url[i]!='/') i--;
	return url + i+1;
}

void pattern_init(machDataT* md, char* pattern)
{
	pattern_reset(md);
	md->pattern = pattern;
	md->len = strlen(pattern);

}

int pattern_match(machDataT* md, char c)
{
	if (md->found)
		return pattern_matchValue(md, c);

	if (c==md->pattern[md->pos])
	{
		md->pos++;
		if (md->pos==md->len)
		{
			md->valuelen=0;
			md->found=1;
			return 1; // we've found the pattern
		}
	} else md->pos=0;
	return 0;
}


int pattern_matchValue(machDataT* md, char c)
{
	if (md->found2)
		return 3;
	if (c==' ' || c=='\t' ) return 0;
	if (c=='\r' || c=='\n' )
	{
		md->value[md->valuelen]=0;
		md->found2 = 1;
		return 2;
	};
	md->value[md->valuelen]=c;
	md->valuelen++;
	if (md->valuelen>=MAXPATTERNLEN) //overflow
	{
		printf("Pattern match buffer has overflown.");	
		exit(0);
	}
	return 1;
}

void pattern_reset(machDataT* md)
{
	md->found = md->found2 =0;
	md->valuelen = 0;
	md->pos=0;
}

void normalisePathSeparators(char* path)
{
	int i, plen;

	plen = strlen(path);

	for (i=3; i<plen; i++) // skiping first 3 characters
	{
		if (path[i]=='/')
			path[i]='\\';
	} //for
} //void normalisePathSeparators(char* path)


// Opens a text file for writing and creates the inttermediary directories if needed
FILE* openFile(const char* path, const char* mode)
{
// Alternatives: SHCreateDirectoryEx function on WinXP SP2 or later
// or #include <boost/filesystem.hpp> and boost::filesystem::create_directories("/some/path");

	char c, buff[MAXPATHLEN];
	int i, len, plen;
	FILE* f;
	
	plen = strlen(path);
	if (plen>MAXPATHLEN-1)
	{
		printf("Filename buffer overflown.");
		exit(0);	
	}

	len = 0;
	strcpy(buff, path);
	for (i=3; i<plen; i++) // skiping first 3 characters
	{
		c = path[i];
		if (c=='\\' || c=='/')
		{
			buff[i]=0;
			CreateDirectory (buff, NULL);
			// no error cecking, we don't care if the directory existsed or it is newly created
			buff[i]='\\';
		}
	}
	if (buff[plen-1]=='\\' || buff[plen-1]=='/')
		buff[plen-1]=0;
	if ( DirectoryExists( buff ))
		strcat(buff, "\\index.html");
	f =  fopen(buff, mode);
	return f;
} //FILE* openFile(const char* path)

int separator(char c)
{
	return (c=='\\' || c=='/' || c==0);
}

int concatenatePath(char*dest, char*src1, char*src2, char*src3, char*src4)
{
	int l1 = strlen(src1);
	int l2 = strlen(src2);
	int l3 = strlen(src3);
	int l4 = strlen(src4);
	int pos;

	if (l1+l2+l3+l4>=MAXPATHLEN)
		return 0;
	strcpy(dest, src1);
	pos = l1;
	if (!separator(dest[pos-1]) && !separator(src2[0]) )
	{
		dest[pos]='\\';
		pos++;
	}
	memcpy(dest+pos, src2, l2);
	pos+=l2;
	if (!separator(dest[pos-1]) && !separator(src3[0]) )
	{
		dest[pos]='\\';
		pos++;
	}
	memcpy(dest+pos, src3, l3);
	pos+=l3;
	if (!separator(dest[pos-1]) && !separator(src4[0]) )
	{
		dest[pos]='\\';
		pos++;
	}
	memcpy(dest+pos, src4, l4+1);
	return 1;
}

int concatenateUrl_worker(char* dest, char*src, int slen)
{ // slen = strlen(src)
	int i, pos;
	pos=0;
	for (i=0; i<slen; i++)
	{
		if (src[i]==' ')
		{
			dest[pos]='%';
			dest[pos+1]='2';
			dest[pos+2]='0';
			pos +=2;
		} else
			dest[pos]=src[i];
		pos++;
	}
	return pos;
}
/*
int concatenateUrl_worker2(char* dest, char*src, int slen)
{ // slen = strlen(src)
	int i, pos;
	pos=0;

	if (slen=0)
		return;
	if (seprator(src[0]))
	{
		if separator(dest[0])

	}
	*/

int concatenateUrl(char*dest, char*src1, char*src2, char*src3, char*src4)
{
	int l1 = strlen(src1);
	int l2 = strlen(src2);
	int l3 = strlen(src3);
	int l4 = strlen(src4);
	int pos;

	if (l1+l2+l3+l4>=MAXPATHLEN)
		return 0;
	
	for (pos=0; pos<100; pos++) dest[pos]=0;

	pos = concatenateUrl_worker(dest, src1, l1);

	if (!separator(dest[pos-1]) && !separator(src2[0]) )
	{
		dest[pos]='/';
		pos++;
	}
	pos += concatenateUrl_worker(dest+pos, src2, l2);
	if (!separator(dest[pos-1]) && !separator(src3[0]) )
	{
		dest[pos]='/';
		pos++;
	}
	pos += concatenateUrl_worker(dest+pos, src3, l3);
	if (!separator(dest[pos-1]) && !separator(src4[0]) )
	{
		dest[pos]='/';
		pos++;
	}
	pos += concatenateUrl_worker(dest+pos, src4, l4);
	dest[pos]=0;
	return 1;
}