#define _CRT_SECURE_NO_WARNINGS


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
//#include <algorithm>
//#include <regex>

using namespace std;

int main_uva1159() // 11559 - Event Planning
{
#ifndef ONLINE_JUDGE
	freopen("uva11559b.in", "r", stdin);
	freopen("uva11559.out", "w", stdout);
#endif

	int N, B, H, W;
	int i, j;
	int ppn, beds;
	int cost, mincost;

	while (scanf("%d %d %d %d\n", &N, &B, &H, &W) == 4)
	{
		mincost = B * 2;
		for (i = 0; i < H; ++i)
		{
			scanf("%d\n", &ppn);
			for (j = 0; j < W; ++j)
			{
				scanf("%d", &beds);
				if (beds >= N)
				{
					cost = N*ppn;
					if (cost < mincost) mincost = cost;
				}
			}
		}
		if (mincost < B)
		{
			printf("%d\n", mincost);
		}
		else
		{
			printf("stay home\n");
		}
	}
	return 0;
}