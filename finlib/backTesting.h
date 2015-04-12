#ifndef _BACKTESTING_H
#define _BACKTESTING_H

#include "market.h"
#include "strategies.h"
#include "trader.h"
#include "timeSeries.h"

double simulate(TimeSeriesFromFile<timeStampedBidAskT>* ts, strategyT* stra, marketT* market, traderT* trader, int minPos, int maxPos);

#endif