#ifndef _STATISTICS_H
#define _STATISTICS_H

#include <math.h>
#include <fstream>
//#include "filesystems.h"
#include "ExcelConversions.h"


class indicatorT : public MotherOfAllBaseObjects
{
public:
	indicatorT (int _capacity) : full(false), capacity(_capacity) {}
	virtual ~indicatorT() {};
	virtual bool isfull() {return full;}
	virtual double value()=0;
	virtual inline void clear() {full=false;}
	virtual void add(int time, double d)=0;
	virtual void update(labelValue& arg) {}

	int capacity;
//
protected:
	bool full;
}; // class indicatorT



class mean : public indicatorT
{
public:
	mean (int _capacity) : indicatorT(_capacity), sumx(0) {}
	virtual double value() {return sumx / capacity;}
	virtual inline void clear() {sumx=0; indicatorT::clear();}
	virtual void add(int time, double d);
	virtual ~mean() {}
	virtual const char* name() {return "mean";}
	static inline double element(int i) { return x[(i+size) % size]; }
	virtual void update(labelValue& arg);

	static int pos;
	static int size;
	static double *x;

protected:
	double sumx;
};

class memory : public mean
{
public:
	memory (int _capacity) : mean(_capacity) {}
	virtual double value();
	virtual ~memory() {}
	virtual const char* name() {return "memory";}
};

class indexmemory : public indicatorT
{
// keeps track of <capacity> last values for <subIndex>
public:
	indexmemory (int capacity, indicatorT* _subIndex) : subIndex(_subIndex), indicatorT(capacity), pos(0) { clear(); values = new double [capacity];}
	void clear();
	virtual ~indexmemory () {delete[] values;}
	virtual double value() {return full? element(pos-capacity) : -1;}
	void indexmemory::add(int time, double d);
	double& element(int pos) {return values[(pos + capacity) % capacity];}
	virtual const char* name() {return "indexmemory";}
protected:
	double* values;
	int pos;
	indicatorT* subIndex;
};

class oa_ema: public indicatorT
{ //exponential moving average
public:
	oa_ema (double _tau ) : indicatorT(1), tau(_tau) { }
	virtual double value() {return sumx;}
	virtual inline void clear() {sumx=0; full=false;}
	virtual void add(int time, double d);
	virtual const char* name() {return "oa_ema";}
	void update(labelValue& arg);

protected:
	double tau;
	double prev_t; // previous time stored
	double sumx;
private:
	mutable double alpha;// temp variable used by add.
};

class ema: public indicatorT
{ //exponential moving average
public:
	ema (double _alpha ) : indicatorT(1), alpha(_alpha) { }
	virtual double value() {return sumx;}
	virtual inline void clear() {sumx=0; full=false;}
	virtual void add(int time, double d);
	virtual const char* name() {return "ema";}
	void update(labelValue& arg);

protected:
	double alpha;
	double sumx;
};

class emai: public indicatorT
{ //itterated exponential moving average
public:
	emai (double _alpha, int _n ) : indicatorT(1), alpha(_alpha), n(_n) { sumx = new double [n]; clear();}
	~emai() {delete[] sumx;}
	virtual double value() {return sumx[n-1];}
	virtual double value(int k) {return sumx[k];}
	virtual inline void clear();
	virtual void add(int time, double d);
	virtual const char* name() {return "emai";}
	void update(labelValue& arg);

protected:
	double alpha;
	int n;
	double* sumx;
};


class oa_emai: public indicatorT
{ //itterated exponential moving average
public:
	oa_emai (double _tau, int _n ) : indicatorT(1), tau(_tau), n(_n) { sumx = new double [n]; clear();}
	~oa_emai() {delete[] sumx;}
	virtual double value() {return sumx[n-1];}
	virtual double value(int k) {return sumx[k];}
	virtual inline void clear();
	virtual void add(int time, double d);
	virtual const char* name() {return "oa_emai";}
	void update(labelValue& arg);

protected:
	int n;
	double* sumx;
	double tau;
	double prev_t;
};



class oa_differential: public indicatorT
{ //differential operator
public:
	oa_differential (double tau, double alpha, double beta, double _gamma) : indicatorT(1), gamma(_gamma), emai2(tau*alpha, 2), emai4(alpha*beta*tau,4) { }
	virtual double value();
	virtual inline void clear() { full=false; emai2.clear(); emai4.clear();}
	virtual void add(int time, double d);
	virtual const char* name() {return "oa_differential";}
//	void update(labelValue& arg);

protected:
	oa_emai emai2, emai4;
	double gamma;
};



class stdev: public indicatorT
{ //exponential moving average
public:
	stdev();
	stdev (int _capacity, indicatorT* _mean) : indicatorT(_capacity), mean(_mean), sumx2(0) { }
	virtual double value();
	virtual inline void clear() {sumx2=0; full=false; mean->clear();}
	virtual bool isfull();
	virtual void add(int time, double d);
	virtual const char* name() {return "stdev";}

protected:
	double sumx2;
	indicatorT* mean;
};

class emadev: public indicatorT
{ //exponential moving stdev
public:
	emadev();
	emadev (double _alpha, indicatorT* _mean) : indicatorT(1), alpha(_alpha), mean(_mean), sumx(0) { }
	virtual double value();
	virtual inline void clear() {sumx=0; full=false; mean->clear();}
	virtual bool isfull()  {return full;}
	virtual void add(int time, double d);
	virtual const char* name() {return "emadev";}

protected:
	double sumx;
	double alpha;
	indicatorT* mean;
};


class summation2
{
public:
	summation2(); // no default constructor
	summation2(int _nbIndicators, indicatorT** _counters);
	virtual inline double value(int n) {return counters[n]->isfull()?counters[n]->value():-1;}
	virtual inline const char* name(int n) {return counters[n]->name();}
	bool inline full(int n) {return counters[n]->isfull();}
	virtual ~summation2()
	{
		if (initialised)
		{
		//	delete[] mean::x;
		}
		delete[] counters;
	}
	void addDataPoint(int ime, double bid, double ask);
	void clear();
	bool full();
	bool visualise(std::ofstream& out, timeStampedBidAskT* rec, int pos, int lo);
	int nbIndicators;
	int nrticks;
	
protected:
	bool initialised;
	indicatorT** counters;

};


summation2* createSummation(labelValue& arg);

indicatorT* createIndicator(labelValue& arg);


#endif