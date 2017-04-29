#ifndef _parser_h_
#define _parser_h_

#define bool int

typedef struct
{
	char* filename; 
	char* website;
	int level;
	char* path; 
	int parent;
} urlT;

urlT* peek(char clow);

#endif