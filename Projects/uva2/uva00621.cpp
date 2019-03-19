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

// we assume string is max 1024, gets AC.
// We assume that all sequences not matched are a "?"
int main_uva00621() // 621 - Secret Research
{
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);

#ifndef ONLINE_JUDGE
	freopen("uva00621.in", "r", stdin);
	freopen("uva00621.out", "w", stdout);
#endif
	int n, len;
	char s[1024];
	scanf("%d\n", &n);
	while (n--)
	{
		scanf("%s", s);
		len = strlen(s);
		if (strcmp(s,"4")==0 || strcmp(s, "1") == 0 || strcmp(s, "78") == 0)
		{
			printf("+\n"); continue;
		}
		if (s[len-2] == '3' && s[len - 1] == '5')
		{
			printf("-\n"); continue;
		}

		if (s[0] == '9' && s[len - 1] == '4')
		{
			printf("*\n"); continue; 
		}
		printf("?\n");
	};
	return 0;
}
