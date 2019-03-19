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


int main_uva12372() // 12372 - Packing for Holiday
{
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);

#ifndef ONLINE_JUDGE
	freopen("uva12372.in", "r", stdin);
	freopen("uva12372.out", "w", stdout);
#endif
	int x, y, z;
	int n, N;
	scanf("%d\n", &N); n = N;
	while (n--)
	{
		scanf("%d %d %d\n", &x, &y, &z);
		if (x > 20 || y > 20 || z > 20) printf("Case %d: bad\n", N - n); else printf("Case %d: good\n", N - n);
	} ;
	return 0;
}
