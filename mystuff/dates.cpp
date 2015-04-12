#define _CRT_SECURE_NO_WARNINGS
#include "dates.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "boost/date_time/gregorian/gregorian.hpp" //include all types plus i/o

#include "boost/date_time/gregorian/gregorian_types.hpp" //no i/o just types

// returns number of days since 1st Jan 1900 (to agree with Excel conventions)
int ExcelDate(int year, int month, int day)
{ 
	static boost::gregorian::date start(1900,1,1);
	boost::gregorian::date d(year,  month,  day);
	boost::gregorian::date_duration duration = d-start;
	return duration.days() + 2; //+2 needed to agree with Excel dates
}


int dateFromString(const char* ptr)
{
	// Matching format 27-Feb-11
	if (ptr[2]!='-' || ptr[6]!='-') throw "Invalid format";
	if (strlen(ptr)!=9) throw "Invalid format";
	
	int day = atoi(ptr);
	int year = atoi(ptr+7);
	char mstr[4];
	int month;
	strncpy(mstr, ptr+3, 3); mstr[3] = 0; 
	if (_strnicmp(mstr, "Jan", 3)==0) month = 0;
	else if (_strnicmp(mstr, "Jan", 3)==0) month = 1;
	else if (_strnicmp(mstr, "Feb", 3)==0) month = 2;
	else if (_strnicmp(mstr, "Mar", 3)==0) month = 3;
	else if (_strnicmp(mstr, "Apr", 3)==0) month = 4;
	else if (_strnicmp(mstr, "May", 3)==0) month = 5;
	else if (_strnicmp(mstr, "Jun", 3)==0) month = 6;
	else if (_strnicmp(mstr, "Jul", 3)==0) month = 7;
	else if (_strnicmp(mstr, "Aug", 3)==0) month = 8;
	else if (_strnicmp(mstr, "Sep", 3)==0) month = 9;
	else if (_strnicmp(mstr, "Oct", 3)==0) month = 10;
	else if (_strnicmp(mstr, "Nov", 3)==0) month = 11;
	else if (_strnicmp(mstr, "Dec", 3)==0) month = 12;
	else throw "Invalid format";

	if (year>2100) throw "Date is out of scope.";
	if (year>1900 && year<2100) return ExcelDate(year, month, day);
	else if (year>50 && year<100) year +=1900; 
	else if (year<50 && year>=0) year +=2000;
	else throw "Date is out of scope.";
	
	return ExcelDate(year, month, day);
} // int dateFromString(char* ptr)
