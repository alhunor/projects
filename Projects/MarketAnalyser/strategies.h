#ifndef _STRATEGIES_H
#define _STRATEGIES_H

#include "statistics.h"
#include "trader.h"

class strategy
{
public:
	strategy();
	strategy(trader* _tra, summationBase * _su) : tra(_tra), su(_su), initialised(false) {}
	bool executeStrategy(double level, int currentTime);
	void setweights(double _w1, double _w2) {w1=_w1; w2=_w2; initialised=true;}
	void clear() {} // nothing to do in a memoryless strategy
	double w1, w2;
protected:
	trader* tra;
	summationBase* su;
	bool initialised;
};

#endif