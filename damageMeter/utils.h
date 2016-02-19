#pragma once
#include <string>
#include <map>
#include <sqlite3.h>

// GLOBAL variables
extern sqlite3 *db;


void trim(std::string& s);
int asInt(const char* s);
float asFloat(const char* s);
unsigned long int asuIntFromHexa(const char* s);

wchar_t* convert(const char* c);

bool isPrefix(const char* prefix, const char* s);

bool checkFlags(unsigned long int attribute, unsigned long int flags);
