#ifndef _STRATEGIES_H
#define _STRATEGIES_H

#include "ExcelConversions.h"
#include "statistics.h"
#include "trader.h"

class strategyT : public MotherOfAllBaseObjects
{
public:
	strategyT();
	~strategyT() {}
	strategyT(traderT* _tra, summation2 * _su) : tra(_tra), su(_su), initialised(false) {}
	virtual void update(labelValue& arg)=0;
	virtual bool executeStrategy(double bid, double ask, int currentTime, int tick) = 0;
	virtual void clear() {su->clear();}
	virtual const char* name() = 0;
	summation2 * su;
protected:
	traderT* tra;
	bool initialised;
};

class rangeTrader : public strategyT 
{
public:
	rangeTrader(traderT* tra, summation2 * su) : strategyT(tra, su) {}
	virtual void update(labelValue& arg);
	virtual bool executeStrategy(double bid, double ask, int currentTime, int tick);
	void setweights(double _w1, double _w2) {w1=_w1; w2=_w2; initialised=true;}
	virtual const char* name() {return "rangeTrader";}
	double w1, w2;
};


class momentsCrossing : public strategyT 
{
public:
	momentsCrossing(traderT* tra, summation2 * su) : strategyT(tra, su), memory(0) {}
	virtual void update(labelValue& arg);
	//void setParams(double _h, double _w2) {histi=h; initialised=true;}
	virtual bool executeStrategy(double bid, double ask, int currentTime, int tick);
	virtual const char* name() {return "momentsCrossing";}
	virtual void clear() {memory = 0; strategyT::clear();}
	double histi;

protected:
	double memory;
};


strategyT* createStrategy(traderT* trader, labelValue& arg);

#endif