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

static inline int cw(int a, int b)
{
	int d = b - a;
	if (d < 0) d += 40;
	return d;
}

static inline int ccw(int a, int b)
{
	int d = a - b;
	if (d < 0) d += 40;
	return d;
}

int main_uva10550() // 10550 - Combination Lock
{
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);

#ifndef ONLINE_JUDGE
	freopen("uva10550.in", "r", stdin);
	freopen("uva10550.out", "w", stdout);
#endif
	int d, f, s, t;
	int res;
	// dial is fixed and numbers are rotating. clockwise and counterclockwise are inverted!!
	while (scanf("%d %d %d %d\n", &d, &f, &s, &t), d || f || s ||t)
	{
		res = 1080; // mandatory turns
		res += 9*(ccw(d, f) + cw(f, s) + ccw(s, t));

		printf("%d\n", res);
	} ;
	return 0;
}
