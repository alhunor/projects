#ifndef _MARKET_H
#define _MARKET_H

#include "ipc.h"
#include "statistics.h"
#include "excelConversions.h"



typedef enum ot {USER_LEVEL, ATMARKET, CANCEL} orderTypeT;

struct HistoricRecordT
{
	int  tick; //ts->rec[tick].time is the real time
	double order;
};

class historyT
{
public:
	historyT() : n(0) {}
	~historyT() {clear();}
	void addd (int tick, double order);
	void toXlfOper(xlw::XlfOper* xlo);
	int getRows() {return n;}
	void clear() {n=0; q.clear();}
protected:
	int n;
	list<HistoricRecordT> q;
};



struct orderT
{
	orderTypeT orderType;
	int tickEntered;
	int timeEntered;
	int lifeSpan; // if an order does not get executed before this moment it will be cancelled
	double level; // level where the order needs to be executed
	int sn; // serial number of the order;
	double qty; // >0 buy, <0 Sell
};

struct confT
{
	bool cancelled; // set to true if an order has been cancelled
	int tick;
	int time;
	int sn;
	double level;
	double qty;
};


class orders
{
public:
	orders(); // no default constructor
	orders(int _buyorders, mqueue<confT>* confirmations_) : buyorders(_buyorders), confirmations(confirmations_), sello(DBL_MAX), buyo(-DBL_MAX) { }
	int addOrder(orderT* order);
	void executeOrder(double bid, double ask, int tick, int currentTime, mqueue<confT>* confirmations); // poat confirmation in queue
	static orderT* newOrder(double buySell, double level, orderTypeT orderType, int tickEntered, int timeEntered, int lifeSpan);
	void cancel(orderT* newo);

	void clear();
	historyT history;

	double sello, buyo;
protected:
	int buyorders;
	list<orderT> orderList;
	mqueue<confT>* confirmations; // points to the same queue as the market
	static int sn; // serial number od the order, auto incremented
	void sendCancelConfirmation(int sn);
	void cancelTimedOut(int currentTime); // cancels timed out orders
	void recomputeMinMax();
}; // class orders


class marketT : public MotherOfAllBaseObjects
{ // hold the order and processes market ticks
public:
	marketT(); // no default constructor
	~marketT() {removeMutex(m);} 
	marketT(int _nbbuyorders, mqueue<orderT>* _requests, mqueue<confT>* _confirmations) : nbbuyorders(_nbbuyorders),
			orders(nbbuyorders, _confirmations), requests(_requests), confirmations(_confirmations ) {m=createMutex("MutexOrders", false); }
	bool marketTick(double bid, double ask, int currentTime, int tick);
	void processRequests();
	void clear() { 	orders.clear(); confirmations->clear(); requests->clear();}
	const char* name() {return "marketT";}
//	inline int count(int buySell) {return orders.count(buySell);} // count buy and sell orders

protected:
	int nbbuyorders;
	MUTEX m; // ptotects manipulation inside orders
	orders orders; // sell and buy orders
	mqueue<orderT>* requests;
	mqueue<confT>* confirmations;
};

#endif