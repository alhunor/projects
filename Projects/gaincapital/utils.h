#ifndef _UTILS_H
#define _UTILS_H

//#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <queue>
#include <windows.h>

// converts dateTime string (e.g "2003-01-01 19:23:37") into number seconds since 1st Jan 1970.
time_t string2time(const char* s);

class delimiter
{
public:
	delimiter() : pos(0) {}
	int analyse(char* s, int len);
	char* field(int i);

protected:
	static const int maxFields = 32;
	bool hasString;
	int pos;
	char* fields[maxFields];
};


#endif
