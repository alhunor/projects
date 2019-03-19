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


int main_12250() // 12250 - Language Detection
{
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);

#ifndef ONLINE_JUDGE
	freopen("uva12250.in", "r", stdin);
	freopen("uva12250.out", "w", stdout);
#endif
	int i, res;
	int n=1;
	char s[16];
	char* hello[] = {"HELLO", "HOLA", "HALLO" , "BONJOUR" , "CIAO" , "ZDRAVSTVUJTE" };
	char* lang[] = { "ENGLISH", "SPANISH", "GERMAN", "FRENCH", "ITALIAN", "RUSSIAN", "UNKNOWN" };
	while (scanf("%s\n", s), strcmp(s, "#")!=0)
	{
		res = 6;
		for (i = 0; i < 6; ++i)
		{
			if (strcmp(s, hello[i]) == 0)
			{
				res = i; break;
			}
		}
		printf("Case %d: %s\n", n, lang[res]);
		++n;
	} ;
	return 0;
}
