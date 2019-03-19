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


int main_uva12577() // 12403 - Hajj-e-Akbar
{
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);

#ifndef ONLINE_JUDGE
	freopen("uva12577.in", "r", stdin);
	freopen("uva12577.out", "w", stdout);
#endif
	int n =1;
	char s[16];
	while (scanf("%s", s), s[0]!='*')
	{
		printf("Case %d: Hajj-e-", n);
		if (s[0] == 'H') 
		{
			printf("Akbar\n");
		}
		else
		{
			printf("Asghar\n");
		}
		++n;
	};
	return 0;
}
