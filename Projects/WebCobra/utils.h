#ifndef _utils_h_
#define _utils_h_

#define MAXPATHLEN 512
#define MAXPATTERNLEN 64

int whitespace(char c);

char* newString(const char* const c);

// allocates new memory and returns a string in which %20s have been converted into spaces
char* normaliseUrlString(const char* const c, int len);

char* fileNameFromUrl(const char* const url, int len);

typedef struct 
{
	char* pattern;
	int pos;
	int len;
	char value[MAXPATTERNLEN];
	int valuelen;
	int found, found2;
} machDataT;

void pattern_init(machDataT* md, char* pattern);
int pattern_match(machDataT* md, char c);
int pattern_matchValue(machDataT* md, char c);
void pattern_reset(machDataT* md);

FILE* openFile(const char* path, const char* mode);
void normalisePathSeparators(char* path);

int concatenatePath(char*dest, char* src1, char*src2, char*src3, char*src4);
int concatenateUrl(char*dest, char*src1, char*src2, char*src3, char*src4);

int DirectoryExists( const char* absolutePath );

#endif