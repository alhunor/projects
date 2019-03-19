#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <set>
#include <algorithm>
//#include <regex>

using namespace std;

static bool leapyear(int y)
{
	//return y % 4 == 0 && (y % 100 != 0 || y % 400 == 0);
	if (y % 4 != 0) return false;
	if (y % 400 == 0) return true;
	return y % 100 != 0;
}

int main_uva00893() //893 - Y3K Problem
{
#ifndef ONLINE_JUDGE
	freopen("uva00893b.in", "r", stdin);
	freopen("uva00893.out", "w", stdout);
#endif

	long int nb, day, month, year, days;
	long int m, y;
	int *mo;
	int daysinmonths[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365}; // till end nov
	int daysinmonths2[] = { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }; // till end nov in leap year


	while (scanf("%d %d %d %d\n", &nb, &day, &month, &year)==4)
	{
		if (nb == 0 && day == 0 && month == 0 && year == 0) break;
		// compute number of days since zero in the date read

		days = day + daysinmonths[month-1]; 
		if (month > 2 && leapyear(year)) days++;
		// now days has number of days since start of <year>
		--year;
		days += year * 365 + (year / 4 - year / 100 + year / 400); // add one day for each leap year till "year"
		days += nb;

		// now we convert back days to date
		y = (days / 146097) * 400;
		days %= 146097; // <400y
		if (days == 0)
		{
			printf("31 12 %04d\n", y);
			continue;
		}
		y += (days / (25*(365 * 4 + 1)-1))*100; // add 100y
		days %= 25 * (365 * 4 + 1) - 1; //<100y
		if (days == 0) 
		{
			printf("30 12 %04d\n",  y);
			continue;
		}

		int sizeOfYear = 365;
		++y;
		while (days > sizeOfYear)
		{
			days -= sizeOfYear;
			++y;
			sizeOfYear = (y % 4 == 0) ? 366 : 365; // this loop is called for last 100 years only, leapyear can be simplified.
		}

		if (leapyear(y)) mo = daysinmonths2; else mo = daysinmonths;
		m = 1;
		while (days > mo[m]) ++m;
		days -= mo[m-1];
		printf("%d %d %04d\n", days, m, y);
	}
	return 0;
}
