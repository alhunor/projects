#pragma once
#include <vector>
#include <set>
#include <map>
#include "currency.h"
#include "dataStructures.h"


typedef enum { Default=0, Linear=1, LogLinear, Cubic} InterpolationType;
typedef enum {T1m, T3m, T6m, T12m}	Tenor;



typedef enum { FXLEVEL=0, FORMULA, INVALIDQUOTE }	fxQuoteStatus;

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
	void set(float _level, fxQuoteStatus _status);
	fxQuoteStatus status;
	float level;
	bool dirty;
	// list of dependencies, if formula
};

class fxTable
{
public:
	fxTable();
	~fxTable();
	void setSpot(FXPair fx, float _level);
	void setFormula(FXPair fx, float _level);
	float getSpot(FXPair fx);
protected:
	int n; // number of currencies is n+1
	fxQuote* fx;
	float* lookup; // used for calculation of FX levels indirectly from several pairs
	bool* visited; // used for calculation of FX levels indirectly from several pairs
	int* prev; // used for calculation of FX levels indirectly from several pairs
	std::map<Currency, std::set<Currency> > fxpairs;
};

