#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <iomanip>

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
	if (y % 4 != 0) return false;
	if (y % 400 == 0) return true;
	return y % 100 != 0;
}

static int daysinmonths[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };
static int daysinmonths2[] = { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 };

static const char* zodiac(int y, int m, int d)
{
	int *mo;
	if (leapyear(y)) mo = daysinmonths2; else mo = daysinmonths;

	int days = d + mo[m - 1];

	if (days <= 20 || days>= 23 + mo[11]) return "capricorn";
	if (days >=21 && days <= 19 + mo[1]) return "aquarius";
	if (days >= 20 + mo[1] && days <= 20 + mo[2]) return "pisces";
	if (days >= 21 + mo[2] && days <= 20 + mo[3]) return "aries";
	if (days >= 21  + mo[3] && days <= 21 + mo[4]) return "taurus";
	if (days >= 22 + mo[4] && days <= 21 + mo[5]) return "gemini";
	if (days >= 22 + mo[5] && days <= 22 + mo[6]) return "cancer";
	if (days >= 23 + mo[6] && days <= 21 + mo[7]) return "leo";
	if (days >= 22 + mo[7] && days <= 23 + mo[8]) return "virgo";
	if (days >= 24 + mo[8] && days <= 23 + mo[9]) return "libra";
	if (days >= 24 + mo[9] && days <= 22 + mo[10]) return "scorpio";
	if (days >= 23 + mo[10] && days <= 22 + mo[11]) return "sagittarius";
	return "error";
}

int main_uva11947() // 11947 - Cancer or Scorpio
{
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);

#ifndef ONLINE_JUDGE
	freopen("uva11947.in", "r", stdin);
	freopen("uva11947.out", "w", stdout);
#endif

	int d, m, y, days;
	int *mo;
	int sizeOfYear;
	
	int n, N;
	cin >> N; n = N; cin.ignore();
	char s[10];
	cout.fill('0');
	while (n--)
	{
		cin.getline(s,10);
		m = (s[0] - '0') * 10 + s[1] - '0';
		d = (s[2] - '0') * 10 + s[3] - '0';
		y = (s[4] - '0') * 1000 + (s[5] - '0') * 100 + (s[6] - '0') * 10+ (s[7] - '0');

		// now add 40 weeks;

		days = d + daysinmonths[m-1]; 
		if (m > 2 && leapyear(y)) days++;
		// now days has number of days since start of <year>
		--y;
		days += y * 365 + (y / 4 - y / 100 + y / 400); // add one day for each leap year till "year"
		days += 40*7;

		// now we convert back days to date
		if (N - n == 85)
		{
			mo = NULL;
		}
		y = (days / 146097) * 400;
		days %= 146097; // <400y
		if (days == 0)
		{
			m = 12; d = 31;
			goto  out;
		}
		y += (days / (25*(365 * 4 + 1)-1))*100; // add 100y
		days %= 25 * (365 * 4 + 1) - 1; //<100y
		if (days == 0) 
		{
			m = 12; d = 30;
			goto  out;
		}

		sizeOfYear = 365;
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
		d = days;
	out:
		cout << (N-n)<<' '<< setw(2)<< m << '/' << setw(2)<< d << '/' << y << ' ' << zodiac(y, m,d)<<'\n';
	}
	return 0;
}
