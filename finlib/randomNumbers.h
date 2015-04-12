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


template<> double rand<Exponential>()
{
	double u=0;
	do
	{
		u = uniformVariate();
	} while (u==0);
	return -log(u);
}

template<> double rand<Normalnversion>()
{
	double u= uniformVariate();
	return normalInverse(u);
}



template<> double rand<NormalBoxMuller>()
{
	double res, x,y, sq;
	do
	{
		x= 2.0 * uniformVariate()-1;
		y= 2.0 * uniformVariate()-1;
		sq = x*x+ y*y;
	} while (sq >=1.0);

	res = x*sqrt(-2.0*log(sq)/sq);
		
	return res;
}


template<> double rand<NormalSummation>()
{
	double res = -6; // substract mean of 12 uniforms
	int i;
	for (i=0; i<12; ++i)
	{
		res += uniformVariate();
	};

	res *= 1; // dividing by (sqrt(1/12)/sqrt(12)) , i.e. stdev(x)/sqrt(n) given by central limit theorem
	return res;
}


template<> double rand<NormalRatioOfUniforms>()
{
	double u,v,x,y,q, res;
	do
	{
		u = uniformVariate();
		v = 1.7156*(uniformVariate()-0.5);
		x = u - 0.449871;
		y = abs(v)+0.386595;
		q = x*x + y*(0.196*y-0,25472*x);
	} while (q>0.27597 && (q>0.27846 || v*v > -4.0*log(u)*u*u));

	res = v/u; // dividing by (sqrt(1/12)/sqrt(12)) , i.e. stdev(x)/sqrt(n) given by central limit theorem
	return res;
}
#endif