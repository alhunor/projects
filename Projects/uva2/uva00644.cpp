#define _CRT_SECURE_NO_WARNINGS


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
//#include <algorithm>
//#include <regex>

using namespace std;

int main_uva00644() // 644 - Immediate Decodability
{
#ifndef ONLINE_JUDGE
	freopen("uva00644.in", "r", stdin);
	freopen("uva00644.out", "w", stdout);
 
#endif
	char buff[11];
	string codes[11];
	int i,j,k, set(1);
	bool b;
	
	i = 0;
	while (scanf("%s", buff)==1)
	{
		codes[i] = buff;
		if (buff[0] == '9')
		{
			b = false;
			for (j = 0; j < i; ++j)
			{
				for (k = 0; k < i; ++k)
				{
					if (j != k && codes[j].find(codes[k]) == 0)
					{
						b = true;
						printf("Set %d is not immediately decodable\n", set);
						break;
					}
				}
				if (b) break;
			}
			if (!b)
			{
				printf("Set %d is immediately decodable\n", set);
			}
			++set;
			i = 0;
		}
		else ++i;
	}

	return 0;
}