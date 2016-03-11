#pragma once
#include "yieldCurve.h"

class fxVolSurf
{
public:
	fxVolSurf() : hasData(false) {}
	fxVolSurf(int _today, InterpolationType _it = Linear) : todayDate(_today), SurfaceDefaultInterpolationType(_it), hasData(false) {}
	int today() { return todayDate; }

protected:
	int todayDate;
	InterpolationType SurfaceDefaultInterpolationType;
	bool hasData;
};

