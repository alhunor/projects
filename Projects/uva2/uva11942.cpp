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

int main_uva11942() // 11942 - Lumberjack Sequencing
{
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);

#ifndef ONLINE_JUDGE
	freopen("uva11942.in", "r", stdin);
	freopen("uva11942.out", "w", stdout);
#endif
	int n;
	bool hi, lo;
	int i, prec, curr;

	
	scanf("%d\n", &n);

	printf("Lumberjacks:\n");
	while (n--)
	{
		hi = lo = true;
		scanf("%d", &prec);
		for (i = 1; i < 10; ++i)
		{
			scanf("%d", &curr);
			if (prec > curr) hi = false;
			if (prec < curr) lo = false;
			prec = curr;
		}
		if (hi || lo) printf("Ordered\n"); else printf("Unordered\n");
	};
	return 0;
}
