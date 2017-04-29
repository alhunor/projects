#include "backTesting.h"
#include "market.h"
#include "strategies.h"
#include "trader.h"



double simulate(TimeSeriesFromFile<timeStampedBidAskT>* ts, strategyT* stra, marketT* market, traderT* trader, int minPos, int maxPos)
{
	int time;
	float bid, ask;

	stra->clear();
	market->clear();
	trader->clear();
	
	for (int i=minPos; i<maxPos; i++)
	{
		time = (int)ts->rec[i].time;
		bid = ts->rec[i].bid;
		ask = ts->rec[i].ask;
		
		stra->su->addDataPoint((int)ts->rec[i].time, ts->rec[i].bid, ts->rec[i].ask);

		stra->executeStrategy(bid, ask, time, i); // executes strategy and post trading request depending on limits and current position	
		market->processRequests(); // post trading requests

		market->marketTick(bid, ask, time, i);	// executes orders and posts confirmations

		trader->stopLoss(bid, ask, time, i);

		trader->processConfirmations(); // processes confirmations to track pnl and currentPosition
	}
	double pnl = trader->reportPnL(bid, ask);

	return pnl;
} // double simulate()
