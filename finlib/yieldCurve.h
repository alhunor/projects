#pragma once
#include <vector>
#include <set>
#include <map>


typedef enum { Default=0, Linear=1, LogLinear, Cubic} InterpolationType;
typedef enum {T1m, T3m, T6m, T12m}	Tenor;
typedef enum {AED=0, AFN, ALL, AMD, ANG, AOA, ARS, AUD, AWG, AZN,
BAM, BBD, BDT, BGN, BHD, BIF, BMD, BND, BOB, BRL, BSD, BTN, BWP, BZD, CAD, CDF, CHF, CLP, CNY,
COP, CRC, CUP, CVE, CZK, DJF, DKK, DOP, DZD, EGP, ERN, ETB, EUR, FJD, FKP, GBP, GEL, GHS, GIP,
GMD, GNF, GTQ, GYD, HKD, HNL, HRK, HTG, HUF, IDR, ILS, INR, IQD, ISK, JMD, JOD, JPY, KES,
KGS, KHR, KMF, KPW, KRW, KWD, KYD, KZT, LAK, LBP, LKR, LRD, LSL, LYD, MAD, MDL, MGA, MKD, MMK,
MNT, MOP, MRO, MUR, MVR, MWK, MXN, MYR, MZN, NAD, NGN, NIO, NOK, NPR, NZD, OMR, PAB, PEN, PGK,
PHP, PKR, PLN, PYG, QAR, RON, RSD, RUB, RWF, SAR, SBD, SCR, SDG, SEK, SGD, SHP, SLL, SOS, SRD,
SYP, SZL, THB, TJS, TMT, TND, TOP, TRY, TTD, TWD, TZS, UAH, UGX, USD, UYU, UZS, VEF, VUV, WST,
XAF, XCD, XOF, XPF, YER, ZAR, ZMW, UNKNOWN}	Currency;


typedef enum { LEVEL=0, FORMULA, INVALIDQUOTE }	fxQuoteStatus;

Currency toCurrency(const char* str);

float addTenorToDate(float date, Tenor tenor);

template <typename T>
inline void swap(T& a, T& b)
{
	T c = a;
	a = b;
	b = c;
}

// Holds DFs for a single currency. Multiple currencies need a yieldcurve per currency
class yieldCurve
{
public:
	yieldCurve() : hasData(false) {}
	yieldCurve(int _today, InterpolationType _it = Linear) : todayDate(_today), CurvedeDefaultInterpolationType(_it), hasData(false) {}
	float df(int date, InterpolationType it = Default);
	void buildFromDFs(std::vector<int>& _dates, std::vector<float>& _dfs);
	float forward(int date, Tenor tenor);
	int today() { return todayDate; }
protected:
	int todayDate, maxDate;
	InterpolationType CurvedeDefaultInterpolationType;
	std::vector<int> dates;
	std::vector<float> dfs;
	bool hasData;
};


struct fxQuote
{
	fxQuote() : status(INVALIDQUOTE), dirty (true) {};
	void set(float _level);
	fxQuoteStatus status;
	float value();
	float level;
	bool dirty;
	// list of dependencies, if formula
};

class fxTable
{
public:
	fxTable();
	void set(Currency fgn, Currency dom, float _level);
	double getSpot(Currency fgn, Currency dom);
protected:
	int n; // number of currencies is n+1
	fxQuote* fx;
	std::map<Currency, std::set<Currency> > fxpairs;
};