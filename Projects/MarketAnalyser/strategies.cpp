#include <iostream>
#include <math.h>
#include "ipc.h"
#include "strategies.h"
#include "trader.h"
#include "parameters.h"

using namespace std;

extern int nrticks;

bool strategy::executeStrategy(double level, int currentTime)
{ // return true if a trade order has been generated
	double ind1, ind2;
	double stdev0;
	int sn; // serial no for conditional orders
	if (!initialised)
		return false; // or throw?
	if (!su->fullAccumulators())	
		return false; // wait until all acumulators are full
	stdev0 = su->stdev(0);
	ind1 = (su->mean(0)-level)/stdev0;
	ind2 = (su->mean(1)-level)/su->stdev(1);
//	cout<<nrticks<<"-->"<<su->mean(0)<<","<<su->stdev(0)<<","<<level<<endl;
//	cout<<su->mean(0)<<ind1<<","<<ind2<<","<<ind3<<endl;
	//ind2=0;
	double signal = (w1*ind1 + w2*ind2);
	
	if (fabs(signal)>parameters::SignalThreshold)
	{
		//cout<<nrticks<<"  "<<signal<<endl;
		sn = tra->requestTransaction(sgn(signal), level, UNCONDITIONAL_ORDER, currentTime);
		tra->requestTransaction(-sgn(signal), level+sgn(signal)*stdev0, sn, currentTime);
		return true;
	}
	return false;
}