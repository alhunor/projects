#pragma once
#include <string>
#include <map>
#include <sqlite3.h>

// GLOBAL variables
extern sqlite3 *db;


void trimQuotes(std::string& s);
void trimStartingWhiteSpaces(std::string& s);

int asInt(const char* s);
float asFloat(const char* s);
unsigned long int asuIntFromHexa(const char* s);

wchar_t* convert(const char* c);

bool isPrefix(const char* prefix, const char* s);

bool checkFlags(unsigned long int attribute, unsigned long int flags);

void saveDatabase(sqlite3 *source, const char* destination);
sqlite3* openDatabase(const char* dbname);


// XXX TODO move these somewhere else
enum atype { Player = 0, Pet, Creature, Vehicle, Item, Nil, Invalid };
typedef unsigned int GUID;