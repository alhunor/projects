#include "yieldCurve.h"
#include <list>
#include <set>
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
	throw "Unknown currency: " + std::string(str);
} // Currency toCurrency(char* str)


FXPair toFXPair(std::string& fgndom)
{
	FXPair fx;
	fx.fgn = toCurrency(fgndom.substr(0, 3).c_str());
	fx.dom = toCurrency(fgndom.substr(3).c_str());
	return fx;
} // FXPair toFXPair(std::string& s)


void fxQuote::set(float _level, fxQuoteStatus _status=FXLEVEL)
{
	if (status == FXLEVEL && status == FORMULA)
	{
		return; // do not overwrite n outright level with a formula
	}
	dirty = false;
	status = _status;
	level = _level;
	// XXX TODO mark all dependencies dirty if _status == LEVEL
}


fxTable::fxTable()
{
	n = (int)UNKNOWN;
	fx = new fxQuote[n*(n + 1) / 2];
	lookup = new float[n + 1];
	visited = new bool[n + 1];
	prev = new int[n + 1];
}

fxTable::~fxTable()
{
	delete[] fx;
	delete[] lookup;
	delete[] visited;
	delete[] prev;
}

void fxTable::setSpot(FXPair fxp, float _level)
{
	if (fxp.fgn == fxp.dom) return;

	if (fxp.fgn < fxp.dom)
	{
		swap(fxp.fgn, fxp.dom);
		_level = 1 / _level;
	}
	int pos = (fxp.fgn - 1)*fxp.fgn / 2+ fxp.dom;
	fx[pos].set(_level);

	fxpairs[fxp.fgn].insert(fxp.dom);
	fxpairs[fxp.dom].insert(fxp.fgn);
} // void fxTable::set(Currency fgn, Currency dom, float _level)



void fxTable::setFormula(FXPair fxp, float _level)
{
	if (fxp.fgn == fxp.dom) return;

	if (fxp.fgn < fxp.dom)
	{
		swap(fxp.fgn, fxp.dom);
		_level = 1 / _level;
	}
	int pos = (fxp.fgn - 1)*fxp.fgn / 2 + fxp.dom;
	fx[pos].set(_level, FORMULA);
} // void fxTable::setFormula(Currency fgn, Currency dom, float _level)


float fxTable::getSpot(FXPair fxp)
{
	if (fxp.fgn == fxp.dom) return 1;
	bool inv = false;
	if (fxp.fgn < fxp.dom)
	{
		swap(fxp.fgn, fxp.dom);
		inv = true;
	}
	int pos = (fxp.fgn - 1)*fxp.fgn / 2 + fxp.dom;

	fxQuote& fxq = fx[pos];
	
	if (fxq.status == FORMULA)
	{
		if (fxq.dirty)
		{
			throw "Recompute dirty pair";
			// XXX TODO compute level based on formula and list of depedencies
		}
	} else if (fxq.status == INVALIDQUOTE)
	{
		// FX level requires combining several quotes, used Dijkstra's shortest path
		// lookup[dom] contains the value of the pair fgndom if known, otherwise -1
		int i;
		for (i = 0; i <= n; ++i)
		{
			lookup[i] = -1; // any invalid value
			visited[i] = false;
		}
		std::list<Currency> tovisit;
		lookup[fxp.fgn] = 1;
		prev[fxp.fgn] = fxp.fgn;
		Currency dom2, fgn2;
		tovisit.push_back(fxp.fgn);
		do
		{
			fgn2 = tovisit.front();
			tovisit.pop_front();
			visited[fgn2] = true;
			for (std::set<Currency>::const_iterator it = fxpairs[fgn2].begin(); it != fxpairs[fgn2].end(); ++it)
			{
				dom2 = *it;
				if (lookup[dom2] != -1) continue;
				lookup[dom2] = lookup[fgn2]*getSpot(FXPair(fgn2, dom2)); // we have a direct quote for fgndom or its inverse
				setFormula(FXPair(fxp.fgn, dom2), lookup[dom2]);
				if (dom2 == fxp.dom) goto _exit;
				if (!visited[dom2])
				{
					tovisit.push_back(dom2);
				}
			}
		} while (!tovisit.empty());
_exit:
		if (dom2 == fxp.dom)
		{
			fxq.level = lookup[dom2];
			fxq.dirty = false;
			fxq.status = FORMULA;
			// XXX TODO record formula.
		} else
		{
			throw "Fx pair is missing";
		}
	}
	return inv ? 1 / fxq.level : fxq.level;
} // float fxTable::value(Currency fgn, Currency dom)
