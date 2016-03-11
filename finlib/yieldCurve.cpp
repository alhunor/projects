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
	default:
		throw "Unrecognized interpolation type";
	}
	return df;
} // float yieldCurve::df(int date, InterpolationType it)


void yieldCurve::buildFromDFs(std::vector<int>& _dates, std::vector<float>& _dfs)
{
	dates = _dates;
	dfs = _dfs;
	maxDate = dates[dates.size() - 1];
	hasData= true;
} // bool yieldCurve::buildFromDFs(std::vector<int>& _dates, std::vector<float>& _dfs)


Currency toCurrency(const char* str)
{
	Currency ccy;
	if (strcmp(str, "EUR") == 0)
	{
		ccy = EUR;
	} else if (strcmp(str, "USD") == 0)
	{
		ccy = USD;
	} else if (strcmp(str, "JPY") == 0)
	{
		ccy = JPY;
	} else if (strcmp(str, "GBP") == 0)
	{
		ccy = GBP;
	} else if (strcmp(str, "CAD") == 0)
	{
		ccy = CAD;
	} else if (strcmp(str, "CHF") == 0)
	{
		ccy = CHF;
	} else throw "Unknown currency";
	return ccy;
} // Currency toCurrency(char* str)
