#include "timeSeries.h"
//#include <stdlib.h>
//#include <stdio.h>
#include <string.h> // for memcpy


bool series::init(int n, float* ptr)
{
	data = new float [n];
	if (!data) return false;
	memcpy(data, ptr, sizeof(float)*n);
	initialised = true;
	return true;
}


series* series::createSeries(int n, float* ptr)
{
	series* s =  new series();
	if (s) 
	{
		s->init (n, ptr);
	} else s=NULL;
	return s;
}