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

int main_uva12015() // 12015 - Google is Feeling Lucky
{
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);

#ifndef ONLINE_JUDGE
	freopen("uva12015.in", "r", stdin);
	freopen("uva12015.out", "w", stdout);
#endif
	int n, N;
	int i, max;
	struct { char name[128]; int rel; } web[10];
	
	scanf("%d\n", &n); N = n;

	while (n--)
	{
		max = -999;
		for (i = 0; i < 10; ++i)
		{
			scanf("%s %d\n", web[i].name, &web[i].rel);
			if (max < web[i].rel) max = web[i].rel;
		}

		printf("Case #%d:\n", N - n);

		for (i = 0; i < 10; ++i)
		{
			if (web[i].rel == max)
			{
				printf("%s\n", web[i].name);
			}
		}
	};
	return 0;
}
