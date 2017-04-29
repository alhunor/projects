#ifndef _TRADER_H
#define _TRADER_H

#include "market.h"
#include "statistics.h"

const int MAXTIME=99999999;

class traderT : public MotherOfAllBaseObjects
{
public:
	traderT (int _shortLimit, int _longLimit, mqueue<orderT>* _requests, mqueue<confT>* _confirmations, marketT* _market);
	~traderT() {}
	int requestTransaction(double qty, double level, orderTypeT orderType, int tickEntered, int timeEntered, int lifespan=MAXTIME);
	bool marketTick(double bid, double ask, int currentTime);
	double reportPnL(double bid, double ask);
	void processRequests(); // reads the request queue and updates the border/sorder lists
	void processConfirmations();
	void clear() {	currentPosition=0; pnl=0; tradingHistory.clear(); positions.clear(); pendingOrders.clear(); pendingShortOrders=0; pendingLongOrders=0;}

	void stopLoss(double bid, double ask, int currentTime, int tick); // called from main looop.
	const char* name() {return "traderT";}
	list<confT>::iterator traderT::minLongPosition();
	list<confT>::iterator traderT::maxShortPosition();

	list<confT> positions; // positions resulting from the orders we have executed.
	list<orderT> pendingOrders; // orders fired to the market and awaiting execution.
	//int nbRequests;
	historyT tradingHistory; // contains all orders executed

protected:
	bool initialised;
	int latency; // number of ticks we will always wait before executing an order
	double shortLimit, longLimit; // number of lots we can sell or buy max
	double pendingShortOrders, pendingLongOrders;
	double currentPosition;
	double pnl;
	mqueue<orderT>* requests;
	mqueue<confT>* confirmations;
	marketT* market;

	bool executeOrder(double bid, double ask, int currentTime);

	void init();
};

#endif