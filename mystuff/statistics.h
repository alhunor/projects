#include <boost/dynamic_bitset.hpp>
#include <math.h>
#include "hash.h"


template<typename T, unsigned int (*selector)(T a)>
int estimateUniqueSymbols(T* const data, const int lo, const int hi, int bins=0)
{
// uses points data[lo..ho]
// returns 1, if dataset has less than 3 elements.
// return the estimated number of unique elements if dataset is larger
	if (hi-lo<3) return 1;

	if (bins==0) bins=(hi-lo)/2;
	boost::dynamic_bitset<> bs(bins);
	int hashcode;

	for(int i=lo; i<=hi; i++)
	{
		hashcode=hash32shift( selector(data[i]) ) % bins;
		bs.set(hashcode);
	}
	int count = bs.count();
    return bins*log((float)bins/(bins-count));
}



// specialisation for T=int
int estimateUniqueSymbols(unsigned int* const data, const int lo, const int hi, int bins=0)
{
// uses points data[lo..ho]
// returns 1, if dataset has less than 3 elements.
// return the estimated number of elements if dataset is larger
	if (hi-lo<3) return 1;

	if (bins==0) bins=(hi-lo)/2;
	boost::dynamic_bitset<> bs(bins);
	int hashcode;

	for(int i=lo; i<=hi; i++)
	{
		hashcode=hash32shift(data[i]) % bins;
		bs.set(hashcode);
	}
	int count = bs.count();
    return bins*log((float)bins/(bins-count));
}



template<typename T>
T average(T* data, int lo, int hi)
// returns the <k>th order element from data[lo..hi]
{
    T sum=0;
	for(int i=lo; i<hi; i++) sum+=data[i];
	return sum/(hi-lo+1);
}


template<typename T, float (*selector)(T a)>
float average(T* data, int lo, int hi)
// returns the <k>th order element from data[lo..hi]
{
    double sum=0;
	for(int i=lo; i<hi; i++) sum+=selector(data[i]);
	return sum/(hi-lo+1);
}
