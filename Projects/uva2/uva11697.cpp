#define _CRT_SECURE_NO_WARNINGS


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
//#include <algorithm>
//#include <regex>

using namespace std;

int main_uva11697() // 11697 - Playfair Cipher
{
#ifndef ONLINE_JUDGE
	freopen("uva11697.in", "r", stdin);
	//freopen("uva11697.out", "w", stdout);
#endif

	int N;
	int i, j, len, lenk;
	char s[1005], key[1005], m[1005];
	char c[27];
	int cx[26], cy[26], abc[26], nbchars;
	// encryption key stored in c[1..25]
	char aux[26][26][2];

	scanf("%d\n", &N);
	while (N--)
	{
		fgets(key, 1002, stdin);
		strcpy(key, "playfair example");
		fgets(s, 1002, stdin);
		len = strlen(s) - 1;
		lenk = strlen(key) - 1;

		// build encryption key in c[1..25]
		for (i = 0; i < 26; ++i) abc[i] = -1;
		nbchars = 0;
		for (i = 0; i < lenk; ++i)
		{
			if (key[i] == ' ') continue;
			if (abc[key[i] - 'a'] != -1) continue;
			nbchars++;
			c[nbchars] = key[i];
			//ci[nbchar]=
			abc[key[i] - 'a'] = nbchars;
		}
		for (i = 0; i < 26; ++i)
		{
			if (i == 'q' - 'a') continue;
			if (abc[i] == -1)
			{
				nbchars++;
				c[nbchars] = i + 'a';
				//ci[nbchar]=
				abc[i] = nbchars;
			}
		}
		c[26] = 0;
		printf("%s\n", c+1);
		int ix, iy, jx, jy, x,y;
		for (i = 0; i < 26; ++i)
		{
			ix = cx[i];
			iy = cy[i];
			if (i == 'q' - 'a') continue;
			for (j = 0; j < 26; ++j)
			{
				if (j == 'q' - 'a') continue;
				if (j == i) continue;
				jx = cx[j];
				jy = cy[j];
				// Rule 2
				if (ix == jx)
				{
					y = (iy + 1) % 5;
					aux[i][j][0] = 0;
				}
			}

		}

	}
	return 0;
}