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

int main_uva12503() // 12503 - Robot Instructions
{
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);

#ifndef ONLINE_JUDGE
	freopen("uva12503.in", "r", stdin);
	freopen("uva12503.out", "w", stdout);
#endif
	int t,n;
	int pos;
	int i, a[102], aux;
	char s[32];

	scanf("%d\n", &t);

	while (t--)
	{
		pos = 0;
		scanf("%d\n", &n);
		for (i = 1; i <= n; ++i)
		{
			fgets(s, 32, stdin);
			if (s[0] == 'L')
			{
				a[i] = -1; 
				--pos;
			} else if (s[0] == 'R')
			{
				a[i] = 1;
				++pos;
			} else // same as ii
			{
				aux = atoi(s + 8);
				a[i] = a[aux];
				pos += a[i];
			}
		}
		printf("%d\n", pos);
	};
	return 0;
}
