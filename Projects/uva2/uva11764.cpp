#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <iomanip>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <set>
#include <algorithm>
//#include <regex>

using namespace std;

int main_uva11764() // 11764 - Jumping Mario
{
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);

#ifndef ONLINE_JUDGE
	freopen("uva11764.in", "r", stdin);
	freopen("uva11764.out", "w", stdout);
#endif
	int n, t, T;
	int hi, lo;
	int i, prec, curr;

	
	scanf("%d\n", &t); T = t;

	while (t--)
	{
		scanf("%d\n", &n);
		hi = lo = 0;
		if (n > 0)
		{
			scanf("%d", &prec);
			for (i = 1; i < n; ++i)
			{
				scanf("%d", &curr);
				if (prec < curr) ++hi;
				if (prec > curr) ++lo;
				prec = curr;
			}
		}
		printf("Case %d: %d %d\n", T-t, hi, lo);
	};
	return 0;
}
