#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string.h>
#include <time.h>
#include "utils.h"
#include <windows.h>


int digit(char c) {return c-48;}

// converts dateTime string (e.g "2003-01-01 19:23:37") into number seconds since 1st Jan 1970.
time_t string2time(const char* s)
{
	tm aux;
	aux.tm_year = digit(s[0])*1000+digit(s[1])*100+digit(s[2])*10+digit(s[3])-1900;
	aux.tm_mon = digit(s[5])*10+digit(s[6])-1;
	aux.tm_mday = digit(s[8])*10+digit(s[9]);

	aux.tm_hour = digit(s[11])*10+digit(s[12]);
	aux.tm_min = digit(s[14])*10+digit(s[15]);
	aux.tm_sec = digit(s[17])*10+digit(s[18]);
	aux.tm_isdst = 0;

	time_t time = mktime(&aux);

	struct tm * ptm;
	ptm = gmtime ( &time );
	
	//std::cout<<s<<"    ->     "<<ptm->tm_year+1900<<"."<<ptm->tm_mon+1<<"."<<ptm->tm_mday<<":"<<ptm->tm_hour<<":"<<ptm->tm_min<<":"<<ptm->tm_sec<<std::endl;
	return time;
}

int delimiter::analyse(char*s, int len)
{	// return number of comma separated fields. 
	// len = strlen(s)+1;
	int i;
	pos = 0;
	fields[0]=s;
	for (i=0; i<len; i++)
	{
		if (s[i]==',')
		{
			s[i]=0;
			pos++;
			if (pos>=maxFields)
				throw ("Fields exceed capacity.");
			fields[pos]=s+(i+1);
		}
	}
	if (len>0)
	{
		pos++;
		fields[pos]=s+len-1; // this line is useless
	}
	return pos;
} // delimiter::analyse

char* delimiter::field(int i)
{
	if (pos==0)
		throw ("Has no string.");
	if (i>=pos)
		throw ("Do not have that many fields.");

	return fields[i];
}
