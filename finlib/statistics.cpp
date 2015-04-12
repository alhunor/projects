#include "ExcelConversions.h"
#include "statistics.h"
#include <iostream>

double* mean::x;
int mean::pos;
int mean::size;

inline double sqr(double d) {return d*d;}

void summation2::clear()
{
	mean::pos=0;
	for(int i=0; i<nbIndicators; i++) 
		counters[i]->clear();
	nrticks = 0;
}


bool summation2::full()
{
	int i;
	for (i=0; i<nbIndicators; i++)
	{
		if (!counters[i]->isfull()) 
			return false;
	}
	return true;
}

summation2::summation2(int _nbIndicators, indicatorT** _counters) :  nbIndicators(_nbIndicators), counters(_counters)
{
	// Contract: all counter capacities should be smaller than size or the object will crash with invalid memory access. 
	int i;
	mean::pos = 0;
	nrticks = 0;
	mean::size = counters[0]->capacity;
	for (i=1;i<nbIndicators; i++)
	{
		if (counters[i]->capacity>mean::size)
		{
			mean::size=counters[i]->capacity;
		}
	}
	mean::x=new double[mean::size];
	initialised=true;
}


void summation2::addDataPoint(int time, double bid, double ask)
{
	int i;
	double d;
	
	d = (bid+ask)/2;
	 //d = sqrt(bid*ask);
	++nrticks;
	if (!initialised)
		return;
	for (i=0; i<nbIndicators; i++)
	{
		counters[i]->add(time, d);
	}
	mean::x[mean::pos]=d;
	if (++mean::pos==mean::size)
		mean::pos=0;
}

void mean::add(int t, double d)
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


void mean::update(labelValue& arg)
{
	int capacity = (int)arg.getNum("capacity");
	if (capacity<= this->capacity)
	{
		//this->capacity = capacity;
	} else
	{
		throw "Implement me!";
		// implement code to increase capacity :: free mean::x and reallocate it to bigger capacity;
		// need to loook into summation as well.
	}
}


double memory::value()
{
	if (!isfull())
	{	
		return -1;
	} else
	{
		return element(pos-capacity);
	}
} // double memory::value()



void indexmemory::clear()
{
	pos = 0;
	full = false;
}

void indexmemory::add(int time, double d)
{
	if (! subIndex->isfull())
		return;
	
	element(pos) = subIndex->value();
	pos++;
	if (pos==capacity)
		full=true;
}

void ema::add(int t, double d)
{
	if (!full)
	{	
		sumx=d;
		full=true;
	} else
	{
		sumx = alpha*sumx + (1-alpha)*d;
	}
} // void ema::add(double d)


void ema::update(labelValue& arg)
{
	double alpha = arg.getNum("alpha");
	this->alpha = alpha;
}




void oa_ema::add(int t, double d)
{
	if (!full)
	{	
		sumx=d;
		full=true;
	} else
	{
		alpha = exp(-(t-prev_t)/tau);
		sumx = (1-alpha)*d + alpha*sumx;
	}
	prev_t=t;
} // void ema::add(double d)


void oa_ema::update(labelValue& arg)
{
	double tau = arg.getNum("tau");
	this->tau = tau;
}



void emai::clear()
{
	int i;
	for(i=0; i<n; ++i) 
	{
		sumx[i]=0; 
	}
	full=false;
}

void emai::add(int t, double d)
{
	if (!full)
	{	
		for (int k=0; k<n; ++k) sumx[k]=d;
		full=true;
		return;
	}
	
	for (int k=0; k<n; ++k)
	{
		sumx[k] = alpha*sumx[k] + (1-alpha)*d;
		d = sumx[k];
	}
} // void emai::add(double d)


void emai::update(labelValue& arg)
{
	double alpha = arg.getNum("alpha");
	int n = (int)arg.getNum("n");
	this->alpha = alpha;
	if (n != this->n)
	{
		this->n= n;
		if (! sumx) delete[] sumx;
		sumx = new double[n];
	}
} //void emai::update(labelValue& arg)


void oa_emai::clear()
{
	int i;
	for(i=0; i<n; ++i) 
	{
		sumx[i]=0; 
	}
	full=false;
}


void oa_emai::add(int time, double d)
{
	if (!full)
	{	
		for (int k=0; k<n; ++k) sumx[k]=d;
		full=true;
	} else
	{
		double alpha = exp(-(time-prev_t)/tau);
		for (int k=0; k<n; ++k)
		{
			sumx[k] = alpha*sumx[k] + (1-alpha)*d;
			d = sumx[k];
		}
	}
	prev_t=time;
} // void oa_emai::add(int time, double d)


void oa_emai::update(labelValue& arg)
{
	double tau = arg.getNum("tau");
	int n = (int)arg.getNum("n");
	this->tau = tau;
	if (n != this->n)
	{
		this->n = n;
		if (! sumx) delete[] sumx;
		sumx = new double[n];
	}
}


void oa_differential::add(int t, double d)
{
	emai2.add(t,d);
	emai4.add(t,d);
	full=true;
}


double oa_differential::value()
{
	double diff;
	diff = gamma*(emai2.value(1)+emai2.value(0)-2*emai4.value());
	return diff;
}

void stdev::add(int t, double d)
{
	double e;
	if (!full)
	{
		sumx2 +=d*d;
		if (mean::pos==capacity-1) 
			full=true;
	} else
	{
		e=mean::element(mean::pos-capacity);
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


void emadev::add(int t, double d)
{
	if (!full)
	{	
		sumx=0;
		full=true;
	} else
	{
		sumx = (1-alpha)*sqr(mean->value()-d) + alpha*sumx;
	}
} // void emadev::add(double d)


double emadev::value()
{
	return sqrt(sumx);
}


bool summation2::visualise(std::ofstream& out, timeStampedBidAskT* rec, int pos, int lo)
{
	//ofstream out("data.dat");
	//out<<"# Times series data"<<endl;
	bool full=true;
	int i;
	for (i=0; i<nbIndicators; i++)
	{
		full &= counters[i]->isfull();
	}
	if (!full) return false;
	out<<rec[pos].time-rec[lo].time<<'\t'<<rec[pos].bid<<'\t';
	for (i=0; i<nbIndicators-1; i++)
	{
		out<<value(i)<<'\t';
	}
	out<<value(nbIndicators-1)<<std::endl;

	return true;
}

indicatorT* createIndicator(labelValue& arg)
{
	const char* name = arg.getStr("indexName").c_str();
	indicatorT* indi = NULL;
	if (_stricmp(name,"mean")==0) // ema, stdev, emadev
	{
		int capacity = (int)arg.getNum("capacity");
		indi = new mean(capacity);
	} else
	if (_stricmp(name,"ema")==0) // ema, stdev, emadev
	{
		double alpha = arg.getNum("alpha");
		indi = new ema(alpha);
	} else
	if (_stricmp(name,"emai")==0) // ema, stdev, emadev
	{
		double alpha = arg.getNum("alpha");
		int n = (int)arg.getNum("n");
		indi = new emai(alpha, n);
	} else
	if (_stricmp(name,"oa_ema")==0) // ema, stdev, emadev
	{
		double tau = arg.getNum("tau");
		indi = new oa_ema(tau);
	} else
	if (_stricmp(name,"oa_differential")==0) // ema, stdev, emadev
	{
		double tau = arg.getNum("tau");
		double alpha = arg.getNum("alpha");
		double beta = arg.getNum("beta");
		double gamma = arg.getNum("gamma");

		indi = new oa_differential(tau, alpha, beta, gamma);
	} else

	if (_stricmp(name,"stdev")==0) // ema, stdev, emadev
	{
		int capacity = (int)arg.getNum("capacity");
		int h = (int)arg.getNum("meanCalculator");


		indicatorT* mc = dynamic_cast<indicatorT*>((MotherOfAllBaseObjects* )handle[h]);	
		if (!mc)
			throw "Handle does not point to underlying <indexT> type.";

		indi = new stdev(capacity, mc);
	} else
	if (_stricmp(name,"emadev")==0) // ema, stdev, emadev
	{
		double alpha = arg.getNum("alpha");
		int h = (int)arg.getNum("meanCalculator");
		indicatorT* mc = dynamic_cast<indicatorT*>((MotherOfAllBaseObjects* )handle[h]);	
		if (!mc)
			throw "Handle does not point to underlying <indexT> type.";
		indi = new emadev(alpha, mc);
	}
	if (_stricmp(name,"memory")==0) // ema, stdev, emadev
	{
		int capacity = (int)arg.getNum("capacity");
		indi = new memory(capacity);
	}
	if (_stricmp(name,"indexmemory")==0) // ema, stdev, emadev
	{
		int capacity = (int)arg.getNum("capacity");
		int h = (int)arg.getNum("subIndex");
		indicatorT* subindex = dynamic_cast<indicatorT*>((MotherOfAllBaseObjects* )handle[h]);	
		if (!subindex)
			throw "Handle does not point to underlying <indexT> type.";
		indi = new indexmemory(capacity, subindex);
	}
	return indi;
} // index* createIndicator(labelValue& arg)