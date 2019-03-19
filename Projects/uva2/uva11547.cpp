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


int main_uva11547() // 11547 - Automatic Answer
{
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);

#ifndef ONLINE_JUDGE
	freopen("uva11547.in", "r", stdin);
	freopen("uva11547.out", "w", stdout);
#endif
	int res;
	int nb, n;
	scanf("%d", &nb);
	while (nb--)
	{
		scanf("%d\n", &n);
		res = n * 63; // 567 / 9
		res += 7492;
		res *= 5; // 235 / 47
		res -= 498;

		res = fabs(res) / 10;

		 printf("%d\n", res % 10);
	} ;
	return 0;
}
