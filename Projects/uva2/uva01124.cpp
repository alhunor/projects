#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <iomanip>

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <set>
#include <algorithm>
//#include <regex>

using namespace std;


int main_uva01124() // 1124 - Celebrity jeopardy
{
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);

#ifndef ONLINE_JUDGE
	freopen("uva01124.in", "r", stdin);
	freopen("uva01124.out", "w", stdout);
#endif

	string s;
	do 
	{
		getline(cin, s);
		if (s.length()>0) cout << s << '\n';
	} while (!cin.eof());
	return 0;
}

/* from internet:
#include<stdio.h>
int main()
{
char a[512];
while(gets(a))
{
puts(a);
}
return 0;
}
*/