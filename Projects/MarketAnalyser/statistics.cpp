#include "statistics.h"
#include <iostream>

double* index::x;
int index::pos;
int index::size;

inline double sqr(double d) {return d*d;}

void summation2::clear()
{
	index::pos=0;
	for(int i=0; i<nbCounters; i++) 
		counters[i]->clear();
}


bool summation2::full()
{
	int i;
	for (i=0; i<nbCounters; i++)
	{
		if (!counters[i]->isfull()) 
			return false;
	}
	return true;
}

summation2::summation2(int _nbCounters, index** _counters) :  nbCounters(_nbCounters), counters(_counters)
{
	// Contract: all counter capacities should be smaller than size or the object will crash with invalid memory access. 
	int i;
	index::pos = 0;
	index::size = counters[0]->capacity;
	for (i=1;i<nbCounters; i++)
	{
		if (counters[i]->capacity>index::size)
		{
			index::size=counters[i]->capacity;
		}
	}
	index::x=new double[index::size];
	initialised=true;
}


void summation2::addDataPoint(double d)
{
	int i;
	if (!initialised)
		return;
	for (i=0; i<nbCounters; i++)
	{
		counters[i]->add(d);
	}
	index::x[index::pos]=d;
	if (++index::pos==index::size)
		index::pos=0;
}

void mean::add(double d)
{
	double e;
	if (!isfull())
	{	
		sumx+=d;
		if (pos==capacity-1) 
			full=true;;
	} else
	{
		e=element(pos-capacity);
		sumx +=d-e;
	}
} // void mean::add(double d)


void ema::add(double d)
{
	if (!full)
	{	
		sumx=d;
		full=true;
	} else
	{
		sumx = alpha*d + (1-alpha)*sumx;
	}
} // void ema::add(double d)


void stdev::add(double d)
{
	double e;
	if (!full)
	{
		sumx2 +=d*d;
		if (pos==capacity-1) 
			full=true;
	} else
	{
		e=element(pos-capacity);
		sumx2 += d*d-e*e;
	}
} // void stdev::add(double d)

double stdev::value()
{
	// stdev with Bessel's correction
	double sumx = mean->value()*mean->capacity;
	return sqrt((sumx2-sumx*sumx/mean->capacity)/(capacity-1));
} // double stdev::value()


 bool stdev::isfull()
{
	return full && mean->isfull();
} //  bool stdev::isfull()


void emadev::add(double d)
{
	if (!full)
	{	
		sumx=0;
		full=true;
	} else
	{
		sumx = alpha*sqr(mean->value()-d) + (1-alpha)*sumx;
	}
} // void emadev::add(double d)


double emadev::value()
{
	return sqrt(sumx);
}


bool summation2::visualise(std::ofstream& out, dbRecordT* rec, int pos, int lo)
{
	//ofstream out("data.dat");
	//out<<"# Times series data"<<endl;
	bool full=true;
	int i;
	for (i=0; i<nbCounters; i++)
	{
		full &= counters[i]->isfull();
	}
	if (!full) return false;
	out<<rec[pos].time-rec[lo].time<<'\t'<<rec[pos].bid<<'\t';
	for (i=0; i<nbCounters-1; i++)
	{
		out<<value(i)<<'\t';
	}
	out<<value(nbCounters-1)<<std::endl;

	return true;
}