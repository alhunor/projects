#ifndef _TRADER_H
#define _TRADER_H

#include "market.h"

struct positionT
{
	double level;
	double time;
	int sn; // serial number of the order which put this position in place.
	int longShort;
};

class trader
{
public:
	trader (int _shortLimit, int _longLimit, mqueue<orderT>* _requests, mqueue<confT>* _confirmations, marketT* _market);
	~trader() {delete[] watchlist;}
	int requestTransaction(int buySell, double level, int conditional, int timeEntered, int timetoCancel=999999999);
	bool marketTick(double bid, double ask, int currentTime);
	void reportPnL(double bid, double ask);
	void processRequests(); // reads the request queue and updates the border/sorder lists
	void processConfirmations();
	void clear() {	currentPosition=0; pnl=0; nbRequests=0; nbWatched=0;}
	void stopLoss(double bid, double ask, int currentTime); // called from main looop.

	positionT* watchlist;
	int nbRequests;
	int nbWatched;

protected:
	bool initialised;
	int latency; // number of ticks we will always wait before executing an order
	int shortLimit, longLimit; // number of lots we can sell or buy max
	int currentPosition;
	double pnl;
	mqueue<orderT>* requests;
	mqueue<confT>* confirmations;
	marketT* market;

	bool executeOrder(double bid, double ask, int currentTime);

	void init();
};

#endif