#pragma once
#include <vector>

typedef enum { Default=0, Linear=1, LogLinear, Cubic} InterpolationType;
typedef enum {T1m, T3m, T6m, T12m}	Tenor;
typedef enum {EUR, USD, JPY, GBP, CAD, CHF}	Currency;

Currency toCurrency(const char* str);

float addTenorToDate(float date, Tenor tenor);

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

