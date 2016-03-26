#include "yieldCurve.h"
#include <vector>



int tenorToMonths(Tenor tenor)
{
	switch (tenor)
	{
	case T1m:
		return 1;
		break;
	case T3m:
		return 3;
		break;
	case T6m:
		return 6;
		break;
	case T12m:
		return 12;
		break;
	default:
		throw "Unknown tenor string";
	}
}

int addTenorToDate(int date, Tenor tenor)
{
	// XXX TODO this is very crude, needs rewriting
	int end = date + tenorToMonths(tenor)*30;
	return end;
}

// Holds DFs for a single currency. Multiple currencies need a yieldcurve per currency
float yieldCurve::forward(int start, Tenor tenor)
{
	int end = addTenorToDate(start, tenor);
	float fwd = (df(start)/df(end)-1 ) / (start - end);
	return fwd;
}


float linterp(int date, int* dates, float* dfs)
{
	int i = 1;
	for (; dates[i] < date; ++i);
	// data falls between dates[i-1]..[i]
	float w = (float)(date - dates[i - 1]) / (dates[i] - dates[i - 1]);
	float df = (1 - w)*dfs[i - 1] + w*dfs[i];
	return df;
}

float loglinterp(int date, int* dates, float* dfs)
{
	int i = 1;
	for (; dates[i] < date; ++i);
	// data falls between dates[i-1]..[i]
	float w = (float)(date - dates[i - 1]) / (dates[i] - dates[i - 1]);
	float df = pow(dfs[i - 1],1-w) + pow(dfs[i], w);
	return df;
}

float yieldCurve::df(int date, InterpolationType it)
{
	if (!hasData) throw "Curve is empty";

	// return 1 if date is in the past
	if (date < todayDate) return 1;

	// return 1 if date is in the past
	if (date > maxDate) throw "Date is off the curve";

	if (it == Default)
	{
		it = CurvedeDefaultInterpolationType;
	}

	float df;
	switch (it)
	{
	case Linear:
		df = linterp(date, dates.data(), dfs.data());
		break;
	case LogLinear:
		df = loglinterp(date, dates.data(), dfs.data());
		break;
	default:
		throw "Unrecognized interpolation type";
	}
	return df;
} // float yieldCurve::df(int date, InterpolationType it)


struct { Currency ccy; char* s; } currencies[150] = {
	{ USD,"USD" },
	{ CHF,"CHF" },
	{ CAD,"CAD" },
	{ GBP,"GBP" },
	{ EUR,"EUR" },
	{ AUD,"AUD" },
	{ JPY,"JPY" },


	{ AED,"AED" },
	{ AFN,"AFN" },
	{ ALL,"ALL" },
	{ AMD,"AMD" },
	{ ANG,"ANG" },
	{ AOA,"AOA" },
	{ ARS,"ARS" },
	{ AWG,"AWG" },
	{ AZN,"AZN" },
	{ BAM,"BAM" },
	{ BBD,"BBD" },
	{ BDT,"BDT" },
	{ BGN,"BGN" },
	{ BHD,"BHD" },
	{ BIF,"BIF" },
	{ BMD,"BMD" },
	{ BND,"BND" },
	{ BOB,"BOB" },
	{ BRL,"BRL" },
	{ BSD,"BSD" },
	{ BTN,"BTN" },
	{ BWP,"BWP" },
	{ BZD,"BZD" },
	{ CDF,"CDF" },
	{ CLP,"CLP" },
	{ CNY,"CNY" },
	{ COP,"COP" },
	{ CRC,"CRC" },
	{ CUP,"CUP" },
	{ CVE,"CVE" },
	{ CZK,"CZK" },
	{ DJF,"DJF" },
	{ DKK,"DKK" },
	{ DOP,"DOP" },
	{ DZD,"DZD" },
	{ EGP,"EGP" },
	{ ERN,"ERN" },
	{ ETB,"ETB" },
	{ FJD,"FJD" },
	{ FKP,"FKP" },
	{ GEL,"GEL" },
	{ GHS,"GHS" },
	{ GIP,"GIP" },
	{ GMD,"GMD" },
	{ GNF,"GNF" },
	{ GTQ,"GTQ" },
	{ GYD,"GYD" },
	{ HKD,"HKD" },
	{ HNL,"HNL" },
	{ HRK,"HRK" },
	{ HTG,"HTG" },
	{ HUF,"HUF" },
	{ IDR,"IDR" },
	{ ILS,"ILS" },
	{ INR,"INR" },
	{ IQD,"IQD" },
	{ ISK,"ISK" },
	{ JMD,"JMD" },
	{ JOD,"JOD" },
	{ KES,"KES" },
	{ KGS,"KGS" },
	{ KHR,"KHR" },
	{ KMF,"KMF" },
	{ KPW,"KPW" },
	{ KRW,"KRW" },
	{ KWD,"KWD" },
	{ KYD,"KYD" },
	{ KZT,"KZT" },
	{ LAK,"LAK" },
	{ LBP,"LBP" },
	{ LKR,"LKR" },
	{ LRD,"LRD" },
	{ LSL,"LSL" },
	{ LYD,"LYD" },
	{ MAD,"MAD" },
	{ MDL,"MDL" },
	{ MGA,"MGA" },
	{ MKD,"MKD" },
	{ MMK,"MMK" },
	{ MNT,"MNT" },
	{ MOP,"MOP" },
	{ MRO,"MRO" },
	{ MUR,"MUR" },
	{ MVR,"MVR" },
	{ MWK,"MWK" },
	{ MXN,"MXN" },
	{ MYR,"MYR" },
	{ MZN,"MZN" },
	{ NAD,"NAD" },
	{ NGN,"NGN" },
	{ NIO,"NIO" },
	{ NOK,"NOK" },
	{ NPR,"NPR" },
	{ NZD,"NZD" },
	{ OMR,"OMR" },
	{ PAB,"PAB" },
	{ PEN,"PEN" },
	{ PGK,"PGK" },
	{ PHP,"PHP" },
	{ PKR,"PKR" },
	{ PLN,"PLN" },
	{ PYG,"PYG" },
	{ QAR,"QAR" },
	{ RON,"RON" },
	{ RSD,"RSD" },
	{ RUB,"RUB" },
	{ RWF,"RWF" },
	{ SAR,"SAR" },
	{ SBD,"SBD" },
	{ SCR,"SCR" },
	{ SDG,"SDG" },
	{ SEK,"SEK" },
	{ SGD,"SGD" },
	{ SHP,"SHP" },
	{ SLL,"SLL" },
	{ SOS,"SOS" },
	{ SRD,"SRD" },
	{ SYP,"SYP" },
	{ SZL,"SZL" },
	{ THB,"THB" },
	{ TJS,"TJS" },
	{ TMT,"TMT" },
	{ TND,"TND" },
	{ TOP,"TOP" },
	{ TRY,"TRY" },
	{ TTD,"TTD" },
	{ TWD,"TWD" },
	{ TZS,"TZS" },
	{ UAH,"UAH" },
	{ UGX,"UGX" },
	{ UYU,"UYU" },
	{ UZS,"UZS" },
	{ VEF,"VEF" },
	{ VUV,"VUV" },
	{ WST,"WST" },
	{ XAF,"XAF" },
	{ XCD,"XCD" },
	{ XOF,"XOF" },
	{ XPF,"XPF" },
	{ YER,"YER" },
	{ ZAR,"ZAR" },
	{ ZMW,"ZMW" } };


void yieldCurve::buildFromDFs(std::vector<int>& _dates, std::vector<float>& _dfs)
{
	dates = _dates;
	dfs = _dfs;
	maxDate = dates[dates.size() - 1];
	hasData= true;
} // bool yieldCurve::buildFromDFs(std::vector<int>& _dates, std::vector<float>& _dfs)


Currency toCurrency(const char* str)
{
	for (int i = 0; i < 150; ++i)
	{
		if (strcmp(str, currencies[i].s) == 0)
		{
			return currencies[i].ccy;
		}
	}
	throw "Unknown currency";
} // Currency toCurrency(char* str)




float fxQuote::value()
{
	if (status == LEVEL)
	{
		return level;
	}
	else if (status == FORMULA)
	{
		if (!dirty)
		{
			return level;
		}
		// XXX TODO compute level based on formula and list of depedencies
		return 0;
	} else
	{
		// XXX TODO deal with INVALIDQUOTE
		return 0;
	}
}

void fxQuote::set(float _level)
{
	dirty = false;
	status = LEVEL;
	level = _level;
	// XXX TODO mark all dependencies dirty
}



fxTable::fxTable()
{
	n = (int)UNKNOWN;
	fx = new fxQuote[n*(n + 1) / 2];
}

void fxTable::set(Currency fgn, Currency dom, float _level)
{
	if (fgn == dom) return;

	if (fgn < dom)
	{
		swap(fgn, dom);
		_level = 1 / _level;
	}
	int pos = (fgn - 1)*fgn / 2+ dom;
	fx[pos].set(_level);

	fxpairs[fgn].insert(dom);
	fxpairs[dom].insert(fgn);
} // void fxTable::set(Currency fgn, Currency dom, float _level)

double fxTable::getSpot(Currency fgn, Currency dom)
{
	if (fgn == dom) return 1;
	bool inv = false;
	if (fgn < dom)
	{
		swap(fgn, dom);
		inv = true;
	}
	int pos = (fgn - 1)*fgn / 2 + dom;
	double level=fx[pos].value();
	return inv ? 1 / level:level;
} // double fxTable::getSpot(Currency fgn, Currency dom)