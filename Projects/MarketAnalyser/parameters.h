#ifndef _PARAMETERS_H
#define _PARAMETERS_H

#define MARKETLEVELS true
//#define DEBUG

namespace parameters
{
// Counters - might be merged with strategy
	const int CounterLength1 = 80;
	const int CounterLength2 = 200;
	const int CounterLength3 = 300;
	const int BufferLength = 400;
//strategy dependent
	const double w1=0.5;
	const double w2=0;
	const double SignalThreshold = 2;

//trader dependent
	const int StopLossPnL = -10; // stop trading if losses exceed this limit
	const int ShortLimit=-3;
	const int LongLimit=3;
	const double positionStopLoss=-2;

//market
	const int Latency=000;
	const int MaxOrders=6;
}

inline int sgn(double d) { return d<0?-1:1;}

#endif