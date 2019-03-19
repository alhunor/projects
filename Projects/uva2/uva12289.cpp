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


int main_uva12289() // 12289 - One-Two-Three
{
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);

#ifndef ONLINE_JUDGE
	freopen("uva12289.in", "r", stdin);
	freopen("uva12289.out", "w", stdout);
#endif
	int i, j, len, res;
	int n=1;
	char s[16], *target[3] = {"one", "two", "three"};
	scanf("%d\n", &n);
	while (n--)
	{
		scanf("%s\n", s);
		for (j = 0; j < 3; ++j)
		{
			res = 0;
			len = strlen(target[j]);
			for (i = 0; i < len; ++i)
			{
				if (s[i] == target[j][i]) ++res;
			}
			if (res >= len-1)
			{
				printf("%d\n", j+1);
				break;
			}
		}
	} ;
	return 0;
}
