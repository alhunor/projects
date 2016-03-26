#pragma once
#include "fxVolSurf.h"
#include "MotherOfAllBaseObjects.h"
#include "yieldCurve.h"
#include <map>

class marketData : public MotherOfAllBaseObjects
{
public:
	marketData(int date) : todayDate(date), fx(NULL) {}
	~marketData();
	bool addCurve(Currency c, yieldCurve* yc);
	bool addFXVolSurface(Currency c, fxVolSurf* fxvol);
	void addFXTable(fxTable *_fxTable);
	int today() { return todayDate; }
	virtual const char* name() { return "marketData";}
	yieldCurve* getCurve(Currency ccy);
	fxTable* getFxTable() { return fx; }

protected:
	int todayDate;
	std::map <Currency, yieldCurve*> ycs;
	std::map <Currency, fxVolSurf*> fxvols;
	fxTable *fx;
};

