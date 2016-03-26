#include "marketData.h"
#include "yieldCurve.h"


bool marketData::addCurve(Currency c, yieldCurve* yc)
{
	if (todayDate != yc->today())
	{
		throw "Inconsistency of dates";
	}
	ycs[c] = yc;
	return true;
}

bool marketData::addFXVolSurface(Currency c, fxVolSurf* fxvol)
{
	if (todayDate != fxvol->today())
	{
		throw "Inconsistency of dates";
	}
	fxvols[c] = fxvol;
	return true;
}

marketData::~marketData()
{
	std::map <Currency, yieldCurve*>::const_iterator it = ycs.begin();
	for (; it != ycs.end(); ++it)
	{
		delete it->second;
	}
	std::map <Currency, fxVolSurf*>::const_iterator it2 = fxvols.begin();
	for (; it2 != fxvols.end(); ++it2)
	{
		delete it2->second;
	}
	// XXX TODO destroy other market data
}

yieldCurve* marketData::getCurve(Currency ccy)
{
	return ycs[ccy];
}

void marketData::addFXTable(fxTable *_fxTable)
{
	if (fx) delete fx;
	fx = _fxTable;
}