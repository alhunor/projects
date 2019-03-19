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

int main() // 12554 - A Special "Happy Birthday" Song!!!
{
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);

	char s[256];
	strcpy(s,"Happy birthday to you Happy birthday to you Happy birthday to Rujia Happy birthday to you");

#ifndef ONLINE_JUDGE
	freopen("uva12554b.in", "r", stdin);
	freopen("uva12554.out", "w", stdout);
#endif
	int n, N;
	int i;
	char singers[102][102];
	char* word[16], * aux;
	scanf("%d\n", &n); N = n;

	while (n--)
	{
		scanf("%s\n", singers[N - n-1]);
	}
	word[0] = s;
	for (i = 0; i < 15; ++i)
	{
		
		aux = strchr(word[i], ' ');
		word[i + 1] = aux + 1;
		*aux = 0;
	}
	for (i = 0; i < ((N+15) / 16)*16; ++i)
	{
		printf("%s: %s\n", singers[i % N], word[i %16]);
	};
	return 0;
}
