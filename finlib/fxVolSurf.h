#pragma once
#include "yieldCurve.h"


// handles vol for a single currency pair
class fxVolSurf
{
public:
	fxVolSurf() : hasData(false) {}
	fxVolSurf(int _today, InterpolationType _it = Linear) : todayDate(_today), SurfaceDefaultInterpolationType(_it), hasData(false) {}
	int today() { return todayDate; }
	double vol(int date, double strike) { return 0.1; }
	double atmVol();
	double deltaNeutralVol();

protected:
	int todayDate;
	InterpolationType SurfaceDefaultInterpolationType;
	bool hasData;
};

