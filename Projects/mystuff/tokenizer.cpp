#include "tokenizer.h"
#include <cstring>
#pragma warning (disable : 4996)

bool tokenizer::init(const char* s, const int len)
// len includes the trailing 0
{
	if (len > allocated)
	{
		clear();
		data = new char[len];
		allocated = len;
	}
	strcpy(data, s);
	data2 = data;
	return true;
};


void tokenizer::tokenize(int offset, char separator)
{
	tokens[0] = data2+offset;
	char* ptr = data2;
	nbtok = 1;
	while (*data2)
	{
		*ptr = *data2++;
		if (*ptr == separator)
		{
			*ptr = 0;
			tokens[nbtok] = ptr + 1;
			++nbtok;
		}
		++ptr;
	}
	tokenized = true;
}


char* tokenizer::slice(char separator)
{
	char* ptr = data2; 
	char* start=data2;
	while (*data2)
	{
		*ptr = *data2++;
		if (*ptr == separator)
		{
			*ptr = 0;
			break;
		}
		++ptr;
	}
	return start;
}

void tokenizer::clear()
{
	if (allocated>0)
	{
		delete[] data;
	}
}

tokenizer::tokenizer(const char* s, int len) : allocated(0), tokenized(false)
{
	if (len == 0) len = strlen(s)+1;
	init(s, len);
}

const char* tokenizer::token(int n)
{
	return tokens[n];
}