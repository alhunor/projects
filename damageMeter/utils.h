#pragma once
#include <string>

void trim(std::string& s);
int asInt(const char* s);
float asFloat(const char* s);
int asIntFromHexa(const char* s);


bool isPrefix(const char* prefix, const char* s);