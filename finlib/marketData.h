#pragma once
#include "fxVolSurf.h"
#include "mystuff/MotherOfAllBaseObjects.h"
#include "yieldCurve.h"
#include <map>
#include <vector>


class marketData : public MotherOfAllBaseObjects
{
public:
	marketData(int date) : todayDate(date), fx(NULL) { }
	virtual ~marketData();
	bool addCurve(Currency c, yieldCurve* yc);
	bool addFXVolSurface(FXPair fxp, fxVolSurf* fxvol);
	void addFXTable(fxTable *_fxTable);
	int today() { return todayDate; }
	virtual const char* name() { return "marketData";}
	yieldCurve* getCurve(Currency ccy);
	fxTable* getFxTable() { return fx; }
	fxVolSurf* getFXVols(FXPair& fxp) { return fxvols[fxp]; }
	double getFxForward(FXPair fxp, int setDate, double fxSpot = -999, double domRate=-999, double fgnRate=-999);
	double df(Currency ccy, int date);

protected:
	int todayDate;
	std::map <Currency, yieldCurve*> ycs;
	std::map <FXPair, fxVolSurf*> fxvols;
	fxTable *fx;
};

