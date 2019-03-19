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


int main_uva12403() // 12403 - Save Setu
{
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);

#ifndef ONLINE_JUDGE
	freopen("uva12403.in", "r", stdin);
	freopen("uva12403.out", "w", stdout);
#endif
	int T, acc,a;
	char s[16];
	scanf("%d\n", &T); 
	acc = 0;
	while (T--)
	{
		scanf("%s", s);
		if (s[0] == 'd') 
		{
			scanf("%d\n", &a);
			acc += a;
		}
		else
		{
			printf("%d\n", acc);
		}
	};
	return 0;
}
