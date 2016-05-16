#define _CRT_SECURE_NO_WARNINGS
#include "dates.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
#include "boost/date_time/gregorian/gregorian_types.hpp" //no i/o just types

// returns number of days since 1st Jan 1900 (to agree with Excel conventions)
int ExcelDate(int year, int month, int day)
{ 
	Does not agree with Excel for dates between 01 jan 1900 and 29 feb 1900. Excel has the mistake, not boost!!
	static boost::gregorian::date start(1900,1,1);
	boost::gregorian::date d(year,  month,  day);
	boost::gregorian::date_duration duration = d-start;
	return duration.days() + 2; //+2 needed to agree with Excel dates
}
*/


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
	if (year>1900 && year<2100) return YMDToExcelDate(year, month, day);
	else if (year>50 && year<100) year +=1900; 
	else if (year<50 && year>=0) year +=2000;
	else throw "Date is out of scope.";
	
	return YMDToExcelDate(year, month, day);
} // int dateFromString(char* ptr)

//http://www.codeproject.com/Articles/2750/Excel-serial-date-to-Day-Month-Year-and-vise-versa

void ExcelDateToYMD(int nSerialDate, int &nYear, int &nMonth, int &nDay)
{
	// Excel/Lotus 123 have a bug with 29-02-1900. 1900 is not a
	// leap year, but Excel/Lotus 123 think it is...
	if (nSerialDate == 60)
	{
		nDay = 29;
		nMonth = 2;
		nYear = 1900;

		return;
	}
	else if (nSerialDate <= 60)
	{
		// Because of the 29-02-1900 bug, any serial date 
		// under 60 is one off... Compensate.
		nSerialDate++;
	}

	// Modified Julian to DMY calculation with an addition of 2415019
	int l = nSerialDate + 68569 + 2415019;
	int n = int((4 * l) / 146097);
	l = l - int((146097 * n + 3) / 4);
	int i = int((4000 * (l + 1)) / 1461001);
	l = l - int((1461 * i) / 4) + 31;
	int j = int((80 * l) / 2447);
	nDay = l - int((2447 * j) / 80);
	l = int(j / 11);
	nMonth = j + 2 - (12 * l);
	nYear = 100 * (n - 49) + i + l;
}

int YMDToExcelDate(int nYear, int nMonth, int nDay)
{
	// Excel/Lotus 123 have a bug with 29-02-1900. 1900 is not a
	// leap year, but Excel/Lotus 123 think it is...
	if (nDay == 29 && nMonth == 02 && nYear == 1900)
		return 60;

	// DMY to Modified Julian calculatie with an extra substraction of 2415019.
	long nSerialDate =
		int((1461 * (nYear + 4800 + int((nMonth - 14) / 12))) / 4) +
		int((367 * (nMonth - 2 - 12 * ((nMonth - 14) / 12))) / 12) -
		int((3 * (int((nYear + 4900 + int((nMonth - 14) / 12)) / 100))) / 4) +
		nDay - 2415019 - 32075;

	if (nSerialDate <= 60)
	{
		// Because of the 29-02-1900 bug, any serial date 
		// under 60 is one off... Compensate.
		nSerialDate--;
	}

	return (int)nSerialDate;
}