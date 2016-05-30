#include "marketData.h"
#include "yieldCurve.h"
#include "currency.h"
#include <fstream>



bool marketData::addCurve(Currency c, yieldCurve* yc)
{
	if (todayDate != yc->today())
	{
		throw "Inconsistency of dates";
	}
	ycs[c] = yc;
	return true;
}

bool marketData::addFXVolSurface(FXPair fxp, fxVolSurf* fxvol)
{
	if (todayDate != fxvol->today())
	{
		throw "Inconsistency of dates";
	}
	fxvols[fxp] = fxvol;
	return true;
}

marketData::~marketData()
{
	std::map <Currency, yieldCurve*>::const_iterator it = ycs.begin();
	for (; it != ycs.end(); ++it)
	{
		delete it->second;
	}
	std::map <FXPair, fxVolSurf*>::const_iterator it2 = fxvols.begin();
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




double marketData::getFxForward(FXPair fxp, int setDate, double fxSpot, double domRate, double fgnRate)
{
	if (fxSpot == -999)
	{
		fxSpot = fx->getSpot(fxp);
	}
	double domDF;
	double fgnDF;

	int spot_date = spotDate(fxp.fgn, fxp.dom, today());
	if (domRate == -999)
	{
		domDF = exp(-(setDate - spot_date) / 365.25*domRate);
	} else
	{
		yieldCurve* domCurve = ycs[fxp.dom];
		domDF = domCurve->df(setDate) / domCurve->df(spot_date);
	}
	if (fgnRate == -999)
	{
		fgnDF = exp(-(setDate - spot_date) / 365.25*fgnRate);
	} else
	{
		yieldCurve* fgnCurve = ycs[fxp.fgn];
		fgnDF = fgnCurve->df(setDate) / fgnCurve->df(spot_date);
	}
	return fxSpot * domDF / fgnDF;
};


double marketData::df(Currency ccy, int date)
{
	yieldCurve* curve = ycs[ccy];
	return curve->df(date);
}
