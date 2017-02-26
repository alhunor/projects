#ifndef RANDOMNUMBERS_H_
#define RANDOMNUMBERS_H_

#include <dSFMT\dSFMT.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "mathProbability.h"

class NormalBoxMuller;
class Normalnversion;
class NormalSummation;
class NormalRatioOfUniforms;
class Exponential;


void initRandomNumbers(int seed);

// return single uniform variate from [0,1)
double uniformVariate();
// return uniform variate vector from [0,1)
void uniformVariateVector(double* dest, int size);

void randVector(double*dataPtr, int size);

extern dsfmt_t dsfmt_global_data;
static bool initialised=false;

//template<class T> double rand<T>() {return 0};
template<typename T> double rand();

// template specialisations
template<> double rand<Exponential>();
template<> double rand<Normalnversion>();
template<> double rand<NormalBoxMuller>();
template<> double rand<NormalSummation>();
template<> double rand<NormalRatioOfUniforms>();

#endif