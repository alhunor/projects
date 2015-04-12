#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <math.h>
#include "ExcelConversions.h"
#include "ipc.h"
#include "strategies.h"
#include "trader.h"
#include "parameters.h"

using namespace std;

const double EPSILON = 0.0000001;




strategyT* createStrategy(traderT* trader, labelValue& arg)
{
	summation2* sa = createSummation(arg);
	const char* name = arg.getStr("strategyName").c_str();
	if (_stricmp(name, "rangeTrader")==0)
	{
		rangeTrader* stra = new rangeTrader(trader, sa);
		double w1 = arg.getNum("w1");
		double w2 = arg.getNum("w2");
		stra->setweights(w1, w2);
		return stra;
	}
	if (_stricmp(name, "momentsCrossing")==0)
	{
		momentsCrossing* stra = new momentsCrossing(trader, sa);
		double histi = arg.getNum("histeresis");
		stra->histi = histi;
		return stra;
	}

	throw "Unknown strategy";
} // strategyT* createStrategy(traderT* trader, labelValue& arg)


summation2* createSummation(labelValue& arg)
{
	int nbIndicators = (int)arg.getNum("nbIndicators");
	char indicatorName [12], destbuff[3] ;
	strcpy_s(indicatorName, 12,  "indicator");
	indicatorT** counters = new indicatorT*[nbIndicators];
	for (int i=1; i<=nbIndicators; i++)
	{
		strcat(indicatorName, _itoa(i, destbuff, 10));
		//counters[i-1] = (indexT*)arg.getObj(indicatorName);
		counters[i-1] = dynamic_cast<indicatorT*>((MotherOfAllBaseObjects* )arg.getObj(indicatorName));	
		if (!counters[i-1])
			throw "Handle does not point to underlying <indexT> type.";
		counters[i-1]->clear();
		indicatorName[9]=0;
	}
	summation2* sa = new summation2(nbIndicators, counters);
	return sa;
} // summation2* createSummation(labelValue& arg)





bool rangeTrader::executeStrategy(double bid, double ask, int currentTime, int tick)
{ // return true if a trade order has been generated
	double ind1, ind2;
	
	int sn; // serial no for conditional orders
	if (!initialised)
		return false; // or throw?
	if (!su->full())	
		return false; // wait until all acumulators are full
	double stdev1 = su->value(1);
	if (stdev1<EPSILON)
		return false;
	double level = 0.5*(bid+ask);
	ind1 = (su->value(0)-level)/stdev1;
//	ind2 = (su->value(2)-level)/su->value(3);
	ind2=0;

	double signal = (w1*ind1 + w2*ind2);
	
	if (fabs(signal)>parameters::SignalThreshold)
	{
		//cout<<nrticks<<"  "<<signal<<endl;
		if (SGN(signal)==-1)
		{
			sn = tra->requestTransaction(sgn(signal), ask, LEVEL, tick, currentTime);
			throw "finish conditional orders.";
			tra->requestTransaction(-sgn(signal), bid-stdev1, LEVEL, tick, currentTime);
		}
		if (SGN(signal)==1)
		{
			sn = tra->requestTransaction(sgn(signal), bid, LEVEL, tick, currentTime);
			tra->requestTransaction(-sgn(signal), ask+stdev1, LEVEL, tick, currentTime);
		}

		return true;
	}
	return false;
} // bool rangeTrader::executeStrategy


void rangeTrader::update(labelValue& arg)
{
	// the update method need to be strategy specific
	void* p;
	// first
	p=arg.getPointer("w1");
	if (p!=NULL)
	{
		w1=*(double*)p;
	}

} // void rangeTrader::update(labelValue arg)



bool momentsCrossing::executeStrategy(double bid, double ask, int currentTime, int tick)
{
	if (su->nrticks<50) 
		return false;
	if (!initialised)
		return false; // or throw?
	if (!su->full())	
		return false; // wait until all acumulators are full
	double mom1 = su->value(0); // short term moment
	double mom2 = su->value(1); // long term moment
	double signal;
//	double level = 0.5*(bid+ask);

	if (mom1<mom2-histi)
	{
		signal = -1; //sell
		if (memory==1)
		{
			tra->requestTransaction(sgn(signal), ask, LEVEL, tick, currentTime);
		}
		memory = -1;
		return true;	
	}
	if (mom1>mom2+histi)
	{
		signal = +1; // buy
		if (memory==-1)
		{
			tra->requestTransaction(sgn(signal), bid, LEVEL, tick, currentTime);
		}
		memory = 1;
		return true;	
	}
	return false;
} // bool momentsCrossing::executeStrategy(double level, int currentTime)



void momentsCrossing::update(labelValue& arg)
{
	// the update method need to be strategy specific
	void* p;
	// first
	p=arg.getPointer("histeresis");
	if (p!=NULL)
	{
		histi=*(double*)p;
	}
	initialised=true;
} // void momentsCrossing::update(labelValue& arg)