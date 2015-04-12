#ifndef _MATHPROBABILITY_H_
#define _MATHPROBABILITY_H_

#include "matrix.h"

int editDistance( char* x,  char* y);
void initialiseCost();
int editDistanceWithCost(char* x, char* y);
//HuMatrix<int> editDistanceWithCost( char* x,  char* y);
int editDistanceWithOverlapDetection( char* x,  char* y);

#endif