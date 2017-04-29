#ifndef _STATISTICS_H
#define _STATISTICS_H

#include <math.h>
#include <fstream>
//#include "filesystems.h"

typedef struct 
{
	time_t time;
	float bid, ask;
} dbRecordT;

class standardAccumulator
{
// used for additions with higher rounding errors
public:
	standardAccumulator() : sum(0) {}
	inline double get() {return sum;}
	inline void add(double d) {sum +=d;} // not made virtual, beware
	inline void clear() {sum=0;}
	
protected:
	double sum;
};

class KahanAccumulator : public standardAccumulator
{
// implements complemented arithmetic, lower rounding errors, see http://en.wikipedia.org/wiki/Compensated_summation.

public:
	KahanAccumulator() : c(0) {}
	inline void add(double d)
	{
		y=d-c; //So far, so good: c is zero.
		t=sum+y; //Alas, sum is big, y small, so low-order digits of y are lost.
		c=(t-sum)-y; //(t - sum) recovers the high-order part of y; subtracting y recovers -(low part of y)
		sum=t;  //Algebraically, c should always be zero. Beware eagerly optimising compilers! 
		//Next time around, the lost low part will be added to y in a fresh attempt.
	}
	inline void clear() {sum=0; c=0;}

protected:
	double c; // A running compensation for lost low-order bits.
	volatile double y,t;
};

template <typename Acc>
class counter
{
public:
	counter () :full(false) {};
	double stdev() {return sqrt((sumx2.get()-sumx.get()*sumx.get()/capacity)/(capacity-1));} // stdev with Bessel's correction
	double mean() {return sumx.get() / capacity;}
	inline void clear() {sumx.clear(); sumx2.clear(); full=false;}
	int capacity;
	bool full;
	Acc sumx, sumx2;
};


class summationBase
{
public:
	summationBase() {}
	virtual inline double stdev(int n) {throw 1;}
	virtual inline double mean(int n)  {throw 1;}
	virtual bool fullAccumulators()  {throw 1;}
};


template <typename Acc> 
class summation : public summationBase
{
public:
	summation(); // no default constructor
	summation(int _nbCounters, int* capacities);
	virtual inline double stdev(int n) {return counters[n].stdev();}
	virtual inline double mean(int n) {return counters[n].mean();}
	bool full(int n) {return counters[n].full;}
	virtual ~summation() {if (initialised) delete[] x;}
	void addDataPoint(double a);
	void report(int i) {cout<<counters[i].getsum()<<"  "<<counters[i].getsum2()<<endl;}
	virtual bool fullAccumulators();
	void clear();
	
protected:
	int size;
	double *x;
	int pos;
	bool initialised;
	int nbCounters;
	counter<Acc>* counters;
	inline double element(int i) { return x[(i+size) % size]; }
	void incrementCounter(int i, double d);
};


template <typename Acc> 
void summation<Acc>::clear()
{
	pos=0;
	for(int i=0; i<nbCounters; i++) 
		counters[i].clear();
}


template <typename Acc> 
bool summation<Acc>::fullAccumulators()
{
	for (int i=0; i<nbCounters; i++)
		if (!counters[i].full) return false;
	return true;
}

template <typename Acc> 
summation<Acc>::summation(int _nbCounters, int* capacities) :  nbCounters(_nbCounters), pos(0)
{
	// Contract: all counter capacities should be smaller than size or the object will crash with invalid memory access. 
	size = capacities[0];
	for (int i=1;i<nbCounters; i++)
	{
		if (capacities[i]>size) size=capacities[i];
	}
	x=new double[size];
	counters=new counter<Acc>[nbCounters];
	for (int i=0; i<nbCounters; i++)
	{
		counters[i].capacity=capacities[i];
	}
	initialised=true;
}

template <typename Acc> 
void summation<Acc>::incrementCounter(int i, double d)
{
	double e;
	if (!counters[i].full)
	{
		counters[i].sumx.add(d);
		counters[i].sumx2.add(d*d);
		if (pos==counters[i].capacity-1) counters[i].full=true;
	} else
	{
		e=element(pos-counters[i].capacity);
		counters[i].sumx.add(d-e);
		counters[i].sumx2.add(d*d-e*e);
	}
}

template <typename Acc> 
void summation<Acc>::addDataPoint(double d)
{
	int i;
	if (!initialised)
		return;
	for (i=0; i<nbCounters; i++)
		incrementCounter(i,d);
	x[pos]=d;
	if (++pos==size) pos=0;
}


class index
{
public:
	index (int _capacity) : full(false), capacity(_capacity) {}
	virtual ~index() {};
	virtual bool isfull() {return full;}
	virtual double value()=0;
	virtual inline void clear() {full=false;}
	virtual void add(double d)=0;
	virtual const char* name()=0;

	static double *x;
	static int pos;
	static inline double element(int i) { return x[(i+size) % size]; }
	static int size;
	int capacity;
//
protected:
	bool full;
};

class mean : public index
{
public:
	mean (int _capacity) : index(_capacity), sumx(0) {}
	virtual double value() {return sumx / capacity;}
	virtual inline void clear() {sumx=0; index::clear();}
	virtual void add(double d);
	virtual ~mean() {}
	virtual const char* name() {return "mean";}

protected:
	double sumx;
};


class ema: public index
{ //exponential moving average
public:
	ema (double _alpha ) : index(1), alpha(_alpha) { }
	virtual double value() {return sumx;}
	virtual inline void clear() {sumx=0; full=false;}
	virtual void add(double d);
	virtual const char* name() {return "ema";}

protected:
	double alpha;
	double sumx;
};


class stdev: public index
{ //exponential moving average
public:
	stdev();
	stdev (int _capacity, index* _mean) : index(_capacity), mean(_mean), sumx2(0) { }
	virtual double value();
	virtual inline void clear() {sumx2=0; full=false; mean->clear();}
	virtual bool isfull();
	virtual void add(double d);
	virtual const char* name() {return "stdev";}

protected:
	double sumx2;
	index* mean;
};

class emadev: public index
{ //exponential moving stdev
public:
	emadev();
	emadev (double _alpha, index* _mean) : index(1), alpha(_alpha), mean(_mean), sumx(0) { }
	virtual double value();
	virtual inline void clear() {sumx=0; full=false; mean->clear();}
	virtual bool isfull()  {return full;}
	virtual void add(double d);
	virtual const char* name() {return "emadev";}

protected:
	double sumx;
	double alpha;
	index* mean;
};


class summation2
{
public:
	summation2(); // no default constructor
	summation2(int _nbCounters, index** _counters);
	virtual inline double value(int n) {return counters[n]->value();}
	bool inline full(int n) {return counters[n]->isfull();}
	virtual ~summation2() {if (initialised) delete[] index::x;}
	void addDataPoint(double a);
	void clear();
	bool inline full();
	bool visualise(std::ofstream& out, dbRecordT* rec, int pos, int lo);
	
protected:
	bool initialised;
	int nbCounters;
	index** counters;
};



#endif