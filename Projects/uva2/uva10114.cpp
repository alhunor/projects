#define _CRT_SECURE_NO_WARNINGS


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <algorithm>
//#include <regex>

using namespace std;

int main_uva10114() //10114 Loansome Car Buyer
{
#ifndef ONLINE_JUDGE
	freopen("uva10114.in", "r", stdin);
	freopen("uva10114.out", "w", stdout);
#endif

	int dur, nb;

	double payed, amount, value, loan, monthly;
	int d, i;
	struct { int m; double f; } dep[105];
	 
	while (scanf("%d %lf %lf %d\n", &dur, &payed, &amount, &nb), dur > 0)
	{
		// read depreciation schedule into dep
		for (i = 0; i < nb; ++i)
		{
			scanf("%d %lf\n", &dep[i].m, &dep[i].f);
		}
		dep[nb].m = dur;  dep[nb].f = dep[nb - 1].f; // sentinel
		monthly = amount / dur;

		// driving off
		loan = amount;
		value = amount + payed;

		d = -1; i = 0;
		do
		{
			++d;
			if (d == dep[i+1].m)
			{
				++i;
			}
			if (d > 0)
			{
				loan -= monthly;
			}
			value *= (1 - dep[i].f);
		} while (loan > value);
		printf("%d month", d);
		if (d != 1) putchar('s');
		putchar('\n');
	}
	return 0;
}
