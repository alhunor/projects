#define _CRT_SECURE_NO_WARNINGS


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
//#include <algorithm>
//#include <regex>

using namespace std;

int main_uva10800() // 10800 - Not That Kind of Graph
{
#ifndef ONLINE_JUDGE
	freopen("uva10800b.in", "r", stdin);
	freopen("uva10800.out", "w", stdout);
#endif

	int nb, N;
	int y;
	int i, j, len;
	char s[52], a[55][55], pos[55];

	scanf("%d\n", &nb);
	N = nb;
	while (nb--)
	{
		scanf("%s", s);
		len = strlen(s);
		y = 1;
		int miny = 99;
		int maxy = -99;
		for (i = 0; i < len; ++i)
		{
			a[0][i] = '-';
			if (s[i] == 'R')
			{
				if (y > maxy) maxy = y;
				if (y < miny) miny = y;
				++y;
			}
			if (s[i] == 'F')
			{
				--y;
				if (y < miny) miny = y;
				if (y > maxy) maxy = y;
			}
			if (s[i] == 'C')
			{
				if (y > maxy) maxy = y;
				if (y < miny) miny = y;
			}
		}

		for (i = 1+maxy-miny; i > 0; --i)
		{

			for (j = 1; j < len + 2; ++j)
			{
				a[i][j] = ' ';
				a[i][0] = '|';
			}
			a[i][len + 2] = 0;
		}
		a[0][0] = '+'; a[0][len] = '-'; a[0][len + 1] = '-'; a[0][len + 2] = '-'; a[0][len + 3] = 0;

		printf("Case #%d:\n", N-nb);
		y = 2-miny;
		for (i = 0; i < len; ++i)
		{
			switch (s[i])
			{
			case 'R':
				a[y][2 + i] = '/';
				pos[y] = 2 + i;
				++y;
				break;
			case 'F':
				--y;
				a[y][2 + i] = '\\';
				pos[y] = 2 + i;
				break;
			case 'C':
				a[y][2 + i] = '_';
				pos[y] = 2 + i;
				break;
			}
		}

		for (i = 1+maxy-miny; i > 0; --i)
		{
			a[i][pos[i] + 1] = 0;
			printf("%s\n", a[i]);
		}
		printf("%s\n\n", a[0]);
	}
	return 0;
}