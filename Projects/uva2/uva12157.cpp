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

int main_uva12157() // 12157 - Tariff Plan
{
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);

#ifndef ONLINE_JUDGE
	freopen("uva12157.in", "r", stdin);
	freopen("uva12157.out", "w", stdout);
#endif
	int n, t, T;
	int i, call;
	int mile, juice;
	
	scanf("%d\n", &t); T = t;

	while (t--)
	{
		scanf("%d\n", &n);
		if (t == 0)
		{
			t = 0;
		}
		mile = juice = 0;
		for (i = 0; i < n; ++i)
		{
			scanf("%d", &call);
			mile += (call + 30) / 30;
			juice += (call + 60) / 60;
		}
		mile *= 10;
		juice *= 15;

		printf("Case %d: ", T - t);
		if (mile == juice) printf("Mile Juice %d\n", mile); else
		if (mile < juice) printf("Mile %d\n", mile); else printf("Juice %d\n", juice);
	};
	return 0;
}
