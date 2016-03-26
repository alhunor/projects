#pragma once

#include <stdlib.h>
#include "matrix.h"

class grid
{
public:
	grid() : x(NULL), t(NULL) {}
	virtual ~grid() { if (x) delete[] x; if (t) delete[] t; }
	int timeSteps, xSteps;
	virtual bool buildGrid(double _x0, double _xmax, double _tmax, int _timeSteps, int _xSteps)=0;
	double *x, *t;
	double x0, xmax, tmax;
	dmat d;
protected:
};

class uniformGrid : public grid
{
public:
	virtual bool buildGrid(double _x0, double _xmax, double _tmax, int _timeSteps, int _xSteps);
protected:
	double dt, dx;
};

class parabolic
{
	double value(double t, double x) { return x*t; }
	double initial(double x) { return x < 0.5 ? 2 * x : 2 - 2 * x; }
	double boundary(double t, double x) { return 0;}
};


template <class gridType, class function>
class solver
{
	gridType grid;
	function f;
	void init(double _x0, double _xmax, double _tmax, int _timeSteps, int _xSteps);
};


template <class gridType, class function>
void solver<typename gridType, typename function>::init(double _x0, double _xmax, double _tmax, int _timeSteps, int _xSteps)
{
	grid.buildGrid(_x0, _xmax, _tmax, _timeSteps, _xStep);
	// apply initial conditions
	double x;
	int i = 0
	for (x = _x; x < _xmax; x += dx)
	{
		d(0, i) = f.initial(x);
		++i;
	}
}