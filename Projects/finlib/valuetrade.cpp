
#include "dataStructures.h"
#include "marketData.h"
#include "myStuff/Matrix.h"
#include "valueTrade.h"
#include <math.h>

using namespace std;


trade::trade(labelValue& lv)
{
	notionalCCy = lv.getCurrency("NotionalCcy");
	notional = lv.getNum("Notional");
}

bool fxForwardTrade::check()
{
	if (notionalCCy != fxp.fgn) throw "NotionalCurrency must be the FGN currency of the FxPair";
	return trade::check();
}

bool time::init(FXPair& fxp, int time, marketData* md, double fxSpot=-999, double fgnRate=-999, double domRate=-999)
{
	t = time;

	if (domRate != -999)
	{
		domDF = exp(-(t - md->today()) / 365.25*domRate);
	}
	else
	{
		domDF = md->df(fxp.dom, t);
	}

	if (fgnRate != -999)
	{
		fgnDF = exp(-(t - md->today()) / 365.25*fgnRate);
	}
	else
	{
		fgnDF = md->df(fxp.fgn, t);
	}
	fxForward = md->getFxForward(fxp, t, fxSpot, domDF, fgnDF);

	return true;
}

bool calendarline::init(FXPair& fxp, labelValue& lv, marketData* md)
{
	double domRate = lv.getNum("domRate", -999);
	double fgnRate = lv.getNum("fgnRate", -999);
	double fxSpot = lv.getNum("fxSpot", -999);

	int t = spotDate(fxp.fgn, fxp.dom, md->today());
	spot.init(fxp, t, md, fxSpot, fgnRate, domRate);

	int setDate = (int)lv.getNum("SetDate");
	expiry.init(fxp, setDate, md, fxSpot, fgnRate, domRate);

	if (lv.getPointer("DeliveryDate") != NULL)
	{
		int deliveryDate = (int)lv.getNum("DeliveryDate");
		delivery.init(fxp, deliveryDate, md, fxSpot, fgnRate, domRate);
	} else
	{
		delivery = expiry;
	}

	return true;
}


fxForwardTrade::fxForwardTrade(labelValue& lv, marketData* md) : trade(lv), fxp(lv.getFXPair("FXPair"))
{
	if (notionalCCy != fxp.fgn) return; // faile initilisation

	cal.init(fxp, lv, md);

	strike = lv.getNum("Strike");

	initialised = true;
}

VariantArray valueFXForward(labelValue& lv, marketData* md, const char* cs, fxForwardTrade* trade)
{
	if (trade == NULL)
	{
		trade = new fxForwardTrade(lv, md);
	}
	if (! trade->check()) return  VariantArray("Invalid trade.");

	if (_strcmpi(cs, "PV") == 0)
	{
		double price = trade->price(); 
		return VariantArray(price);
	}
	else if (_strcmpi(cs, "AllRisks") == 0)
	{
		VariantArray va(2,2);
		va(0, 0) = "Delta";
		va(0, 1) = trade->delta();
		va(1, 0) = "Vega";
		va(1, 1) = 0;
		return va;
	}


	return VariantArray("Unsupported");
} // VariantArray valueFXForward(labelValue& lv, marketData* md, const char* cs)



fxVanillaOption::fxVanillaOption(labelValue& lv, marketData* _md) : trade(lv), fxp(lv.getFXPair("FXPair"))
{
	if (notionalCCy != fxp.fgn) return; // faile initilisation

	strike = lv.getNum("Strike");
	tipus = (FXOptionType)(int)lv.getNum("PayoffType");
	md = _md;

	cal.init(fxp, lv, md);
	initialised = true;
}


bool fxVanillaOption::check() // checks if object is in okay state, throws error
{
	if (notionalCCy != fxp.fgn) throw "NotionalCurrency must be the FGN currency of the FxPair";
	return trade::check();
}

double fxVanillaOption::price()
{
/*	fxVolSurf* fxvsurf = md->getFXVols(fxp);
	vol = fxvsurf->vol(cal.expiry.t, strike);
	double todayFX = md->getFxForward(fxp, md->today);
	double rd_t = log(cal.delivery.domDF / cal.spot.domDF);
	double rf_t = log(cal.delivery.fgnDF / cal.spot.fgnDF);
	double p = notional* fxblackscholes(todayFX, strike, vol, rd_t, rf_t, cal.expiry.t - md->today, cal.delivery.t - cal.spot.t, tipus);
*/
	throw "wrong";

	return 0;
}

double fxVanillaOption::delta()
{
	// TODO XXX wrong formula
	return 0;
}




VariantArray valueTrade(labelValue& lv, marketData* md, std::string& cs, trade* trade)
{
	std::string pc = lv.getStr("ProductCode");
	if (_strcmpi(pc.c_str(), "F") == 0)
	{
		return valueFXForward(lv, md, cs.c_str(), (fxForwardTrade*)trade);
	}
	return VariantArray("Unsupported");
} // VariantArray valueTrade(labelValue& lv, marketData* md, std::string& cs)
