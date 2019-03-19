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


int main_uva11044() // 11044 - Searching for Nessy
{
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);

#ifndef ONLINE_JUDGE
	freopen("uva11044.in", "r", stdin);
	freopen("uva11044.out", "w", stdout);
#endif
	int m,n;
	int nb;
	int res;
	scanf("%d", &nb);
	while (nb--)
	{
		scanf("%d %d\n", &m, &n);	
		res = (m / 3) * (n / 3);

		printf("%d\n", res);
	} ;
	return 0;
}
