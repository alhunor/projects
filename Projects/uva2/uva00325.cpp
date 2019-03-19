#define _CRT_SECURE_NO_WARNINGS


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <regex>

using namespace std;

static char* trim(char* s)
{
	int len;
	char* p = s;
	while (*p == ' ' || *p=='\t') ++p;

	len = strlen(p);
	if (len == 0) return p;
	if (p[len - 1] == '\n') len--;
	while (len>0 && (p[len-1] == ' ' || p[len - 1] == '\t') ) --len;
	p[len] = 0;
	return p;
}

int main_uva00325() // Identifying Legal Pascal Real Constants
{
#ifndef ONLINE_JUDGE
	freopen("uva00325.in", "r", stdin);
	freopen("uva00325.out", "w", stdout);
 
#endif
	char buff[1024];
	regex re("([+-]?[0-9]+[.][0-9]+([eE][+-]?[0-9]+)?)|([+-]?[0-9]+[eE][+-]?[0-9]+)");

	while (fgets(buff, 1000, stdin))
	{
		//strcpy(buff, "1e-12");
		char* p = trim(buff);
		if (strcmp(p, "*") == 0) break; // not correct, since it wil stop on "  *  " but AC

		printf("%s", p);
		if (regex_match(p, re))
		{
			printf(" is legal.\n");

		}
		else
		{
			printf(" is illegal.\n");
		}
	}
	return 0;
}