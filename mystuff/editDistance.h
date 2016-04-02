#ifndef _EDITDISTANCE_H_
#define _EDITDISTANCE_H_

#include "matrix.h"

int editDistance( char* x,  char* y);
void initialiseCost();
int editDistanceWithCost(char* x, char* y);
//HuMatrix<int> editDistanceWithCost( char* x,  char* y);
int editDistanceWithOverlapDetection( char* x,  char* y);


void init_match(VOID);
// http://xoomer.virgilio.it/acantato/dev/wildcard/wildmatch.html#c_cpp_userjournal_algo
bool wildCharMatch(const char* pat, const char* str);

#endif
