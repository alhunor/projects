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

int main_uva10963() // 10963 - The Swallowing Ground
{
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);
// We assume the gap can be closed if the width of the ravine is the same in every column.

#ifndef ONLINE_JUDGE
	freopen("uva10963.in", "r", stdin);
	freopen("uva10963.out", "w", stdout);
#endif
	int n, w, i, lo, hi, gap;
	scanf("%d\n", &n);
	bool first = true;
	bool poss;

	while (n--)
	{
		scanf("%d\n", &w);
		poss = true;
		if (w > 0)
		{
			scanf("%d %d\n", &hi, &lo);
			gap = hi - lo;
			for (i = 1; i < w; ++i)
			{
				scanf("%d %d\n", &hi, &lo);
				if (gap != hi - lo) poss = false;
			}
		}

		if (!first)
		{
			printf("\n");
		}
		if (poss) printf("yes\n"); else printf("no\n");
		first = false;
	};
	return 0;
}
