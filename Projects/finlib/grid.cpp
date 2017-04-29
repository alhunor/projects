#include "grid.h"

bool uniformGrid::buildGrid(double _x0, double _xmax, double _tmax, int _timeSteps, int _xSteps)
{
	x0 = _x0;
	xmax = _xmax;
	tmax = _tmax;
	timeSteps = _timeSteps;
	xSteps = _xSteps;

	dt = (tmax) / timeSteps;
	dx = (xmax-x0) / xSteps;
	x = new double[xSteps];
	t = new double[timeSteps];
	int i;
	for (i = 0; i < timeSteps; ++i)
	{
		t[i] = i*dt;
	}
	for (i = 0; i < xSteps; ++i)
	{
		x[i] = i*dx;
	}
	d.resize(timeSteps, xSteps);
	return true;
}