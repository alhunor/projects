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

static int f(int n)
{
	int sum;
again:
	sum = 0;
	while (n > 0)
	{
		sum += n % 10;
		n /= 10;
	}
	if (sum > 9)
	{
		n = sum;
		goto again;
	}
	return sum;
}

int main_uva11332() // 11332 - Summing Digits
{
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);

#ifndef ONLINE_JUDGE
	freopen("uva11332.in", "r", stdin);
	freopen("uva11332.out", "w", stdout);
#endif
	int n;
	while (scanf("%d\n", &n), n)
	{
		printf("%d\n", f(n));
	};
	return 0;
}
