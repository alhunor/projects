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

int main_uva11679() // 11679 - Sub-Prime
{
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);

#ifndef ONLINE_JUDGE
	freopen("uva11679.in", "r", stdin);
	freopen("uva11679.out", "w", stdout);
#endif
	int n, b, i;
	int r[21]; // reserves
	bool solvent;
	int debtor, creditor, value;
	
	while (scanf("%d %d\n", &b, &n) , (n || b))
	{
		for (i = 1; i <= b; ++i)
		{
			scanf("%d", &r[i]);
		}
	
		for (i = 0; i < n; ++i)
		{
			scanf("%d %d %d\n", &debtor, &creditor, &value);
			r[creditor] += value;
			r[debtor] -= value;
		}
		solvent = true;
		for (i = 1; i <= b; ++i)
		{
			if (r[i] < 0) solvent = false;
		}
		if (solvent) printf("S\n"); else printf("N\n");
	};
	return 0;
}
