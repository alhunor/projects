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


int main_uva11364() // 11364 - Parking
{
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);

#ifndef ONLINE_JUDGE
	freopen("uva11364.in", "r", stdin);
	freopen("uva11364.out", "w", stdout);
#endif
	int i;
	int nb, n, x, minx, maxx;
	scanf("%d", &nb);
	// he can park anywhere between the minimum and the maximum coordinate, then walk to min, to max and  back to the car
	// distance is 2* (maxx-minx).
	while (nb--)
	{
		scanf("%d\n", &n);
		maxx = -1;
		minx = 101;
		for (i = 0; i < n; ++i)
		{
			scanf("%d", &x);
			if (minx > x) minx = x;
			if (maxx < x) maxx = x;

		}
		printf("%d\n", 2*(maxx-minx));
	} ;
	return 0;
}
