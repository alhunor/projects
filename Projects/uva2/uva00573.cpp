#define _CRT_SECURE_NO_WARNINGS


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <algorithm>
//#include <regex>

using namespace std;

int main_uva00573() //573 - The Snail
{
#ifndef ONLINE_JUDGE
	freopen("uva00573.in", "r", stdin);
	freopen("uva00573.out", "w", stdout);
#endif

	int H, U, D, F;

	double h, u;
	int d;

	while (scanf("%d %d %d %d\n", &H, &U, &D, &F), H !=0)
	{
		h = 0;
		d = 1;
		u = U;
		while (true)
		{
			if (d > 1) u = std::max(u - U*F / 100., 0.);
			h += u;
			if (h > H) break;
			h -= D;
			if (h < 0) break;
			++d;
		}
		if (h < 0)
		{
			printf("failure on day %d\n", d);
		} else
		{
			printf("success on day %d\n", d);
		}
	}
	return 0;
}