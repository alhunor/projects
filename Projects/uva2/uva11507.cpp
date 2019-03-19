#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <algorithm>
//#include <regex>

using namespace std;

int main_UVA11507() //11507 - Bender B. Rodríguez Problem
{
#ifndef ONLINE_JUDGE
	freopen("uva11507.in", "r", stdin);
	freopen("uva11507.out", "w", stdout);
#endif

	int L, i;
	char s[4];
	int dir;
	int bendPy[6] = {2,3,1,0,4,5};
	int bendMy[6] = {3,2,0,1,4,5};
	int bendPz[6] = {4,5,2,3,1,0};
	int bendMz[6] = {5,4,2,3,0,1};

	while (scanf("%d\n", &L), L > 0)
	{
		dir = 0; // 0:+x, 1:-x, 2:+y, 3:-y, 4:+z, 5:-z;
		for (i = 0; i < L-1; ++i)
		{
			scanf("%s", s);
			if (strcmp(s, "No") == 0) continue;
			if (strcmp(s, "+y") == 0)
			{
				dir = bendPy[dir];
			} else if (strcmp(s, "-y") == 0)
			{
				dir = bendMy[dir];
			} else if (strcmp(s, "+z") == 0)
			{
				dir = bendPz[dir];
			} else if (strcmp(s, "-z") == 0)
			{
				dir = bendMz[dir];
			};
		}
		switch (dir)
		{
		case 0:
			printf("+x\n"); break;
		case 1:
			printf("-x\n"); break;
		case 2:
			printf("+y\n"); break;
		case 3:
			printf("-y\n"); break;
		case 4:
			printf("+z\n"); break;
		case 5:
			printf("-z\n"); break;
		}
	}
	return 0;
}
