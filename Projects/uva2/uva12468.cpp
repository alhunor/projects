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

int main_uva12468() // 12468 - Zapping
{
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);

#ifndef ONLINE_JUDGE
	freopen("uva12468b.in", "r", stdin);
	freopen("uva12468.out", "w", stdout);
#endif
	int from, to, d;

	while (scanf("%d %d\n", &from, &to), ! (from==-1 && to==-1))
	{
		
		
		if (from <= to) d =  to - from; else d = from-to;
		if (d > 50) d = 100 - d;
		printf("%d\n", d); 
	};
	return 0;
}
