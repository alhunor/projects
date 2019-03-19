#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <set>
#include <algorithm>
//#include <regex>

using namespace std;

int main_uva00489() //489 - Hangman Judge
{
#ifndef ONLINE_JUDGE
	freopen("uva00489.in", "r", stdin);
	freopen("uva00489.out", "w", stdout);
#endif

	int round, i;
	string s, p;
	int h; // hangman characters
	int g; // good chars
	set<char> bad, good, word;

	while (cin>>round, round !=-1)
	{
		cout << "Round " << round << '\n';
		cin.ignore();
		cin >> s >> p;
		h = 0; g = 0;
		bad.clear(); word.clear(); good.clear();
		for (i = 0; i < s.length(); ++i)
		{
			word.insert(s[i]);
		}
		for (i = 0; i < p.length(); ++i)
		{
			if (word.find(p[i]) == word.end())
			{
				// count bad chars only once
				if (bad.find(p[i]) != bad.end()) continue;
				++h; // bad answer
				if (h >= 7)
				{
					cout << "You lose.\n";
					goto next;
				}
				bad.insert(p[i]);
			} else
			{		
				// count good chars only once
				if (good.find(p[i]) != good.end()) continue;
				++g; // good answer
				if (g == word.size())
				{
					cout << "You win.\n";
					goto next;
				}
				good.insert(p[i]);
			}
		}
		cout<<"You chickened out.\n";
	next:;
	}
	return 0;
}
