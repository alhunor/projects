#include "utils.h"
#include <string>

void trim(std::string& s)
{
	if (s[0]=='\"')
		s.erase(0,1);
	int last = s.length() - 1;
	if (s[last] == '\"')
		s.erase(last);
}


int asInt(const char* s)
{
	int i = atoi(s);
	return i;
}

float asFloat(const char* s)
{
	float f = (float)atof(s);
	return f;
}


int asIntFromHexa(const char* s)
{
	int i = strtol(s, NULL,16);
	return i;
}

bool isPrefix(const char* prefix, const char* s)
{
	const char* ptr = prefix;
	while (*ptr)
	{
		if (*ptr != *s++)
		{
			return false;
		}
		++ptr;
	}
	return true;
}