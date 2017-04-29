#include <ctype.h>
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"



urlT* peek(char clow)
{
	static int state = 0;
	static char site[128];
	static int site_len;
	static char path[128];
	static int path_len;
	char c;
	urlT* url;

	
	c=toupper(clow);
	switch (state)
	{
	case 0:
		if (c=='<')
			state=1;
		break;
		// all other characters are ignored
	case 1:
		if (whitespace(c))
			break;
		state = (c=='A') ? 2 : 0;
		break;
	case 2:
		if (whitespace(c)) state=3; else state=0;
		break;
	case 3:
		if (whitespace(c))
			break;
		state = (c=='H') ? 4 : 0;
		break;
	case 4:
		state = (c=='R') ? 5 : 0;
		break;
	case 5:
		state = (c=='E') ? 6 : 0;
		break;
	case 6:
		state = (c=='F') ? 7 : 0;
		break;
	case 7:
		if (whitespace(c))
			break;
		state = (c=='=') ? 8 : 0;
		break;
	case 8:
		if (whitespace(c))
			break;
		if (c=='\"')
		{
			state=9;
			site_len=0;
			path_len=0;
		} else
		state = 0;
		break;
	case 9: //parsing site name
	case 10: // parsing path after site name
		if (c=='\"')
		{
			site[site_len]=0;
			path[path_len]=0;
			state=0;
			url = (urlT*)malloc(sizeof(urlT));
			url->website = newString(site);
			url->filename = newString(path);
			return url;
		}
		if (c=='\\')
		{
			state=10;
			return NULL;
		}
		if (state==9)
		{
			site[site_len++]=clow;
		} else
		{
			path[path_len++]=clow;
		}
		break;
	default: // this case should never arrise unless the memory gets corrupted
		return NULL;
	} // switch (state)
	return NULL;
} // void htmlParser::peek(char c)