#include "randomNumbers.h"
#include <string.h>
#include <gsl\gsl_sf_gamma.h>
#include <dSFMT\dSFMT.h>


void randVector(double*dataPtr, int size)
{
	int i;
	uniformVariateVector(dataPtr, size);
	for (i=0; i<size; ++i)
	{
		dataPtr[i] = normalInverse(dataPtr[i]);
	}
	rand<NormalBoxMuller>();
	rand<Normalnversion>();
	rand<NormalSummation>();
	rand<NormalRatioOfUniforms>();
	rand<Exponential>();
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



