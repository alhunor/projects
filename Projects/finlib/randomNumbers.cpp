#include <string.h>
#include <gsl\gsl_sf_gamma.h>
#include <dSFMT\dSFMT.h>
#include "randomNumbers.h"

void randVector(double*dataPtr, int size)
{
	int i;
	uniformVariateVector(dataPtr, size);
	for (i=0; i<size; ++i)
	{
		dataPtr[i] = normalInverse(dataPtr[i]);
	}
/*	rand<NormalBoxMuller>();
	rand<Normalnversion>();
	rand<NormalSummation>();
	rand<NormalRatioOfUniforms>();
	rand<Exponential>();
*/
}


void dsfmt_genRandVector(dsfmt_t* dsfmt, double* dest, int size)
{
	int i;
	int available; // random number in the generators block that have not been used yet
	int togo; // random numbers still needed

	double *psfmt64 = & dsfmt->status[0].d[0];

	togo = size;

	double* buff = dest;
	do
	{
		available = DSFMT_N64 - dsfmt->idx;
		if (togo<available)
		{
			memcpy(buff, &psfmt64[dsfmt->idx], togo * sizeof(double));
		} else
		{
			memcpy(buff, &psfmt64[dsfmt->idx], available * sizeof(double));
			buff += available;
			dsfmt_gen_rand_all(dsfmt);
			dsfmt->idx = 0;
			togo -= available;
		}
	} while (togo>0);
	for (i=0; i<size; ++i) dest[i]-=1; // substract 1, as the generator return in the [1,2) range;
}

void initRandomNumbers(int seed)
{
	dsfmt_init_gen_rand(&dsfmt_global_data, seed);
	initialised = true;
}

// return single uniform variate from [0,1)
inline double uniformVariate() {return dsfmt_gv_genrand_close_open();}

// return uniform variate vector from [0,1)
inline void uniformVariateVector(double* dest, int size)
{
	dsfmt_genRandVector(&dsfmt_global_data, dest, size);
}


template<> double rand<Exponential>()
{
	double u = 0;
	do
	{
		u = uniformVariate();
	} while (u == 0);
	return -log(u);
}

template<> double rand<Normalnversion>()
{
	double u = uniformVariate();
	return normalInverse(u);
}



template<> double rand<NormalBoxMuller>()
{
	double res, x, y, sq;
	do
	{
		x = 2.0 * uniformVariate() - 1;
		y = 2.0 * uniformVariate() - 1;
		sq = x*x + y*y;
	} while (sq >= 1.0);

	res = x*sqrt(-2.0*log(sq) / sq);

	return res;
}


template<> double rand<NormalSummation>()
{
	double res = -6; // substract mean of 12 uniforms
	int i;
	for (i = 0; i<12; ++i)
	{
		res += uniformVariate();
	};

	res *= 1; // dividing by (sqrt(1/12)/sqrt(12)) , i.e. stdev(x)/sqrt(n) given by central limit theorem
	return res;
}


template<> double rand<NormalRatioOfUniforms>()
{
	double u, v, x, y, q, res;
	do
	{
		u = uniformVariate();
		v = 1.7156*(uniformVariate() - 0.5);
		x = u - 0.449871;
		y = fabs(v) + 0.386595;
		q = x*x + y*(0.196*y - 0, 25472 * x);
	} while (q>0.27597 && (q>0.27846 || v*v > -4.0*log(u)*u*u));

	res = v / u; // dividing by (sqrt(1/12)/sqrt(12)) , i.e. stdev(x)/sqrt(n) given by central limit theorem
	return res;
}

