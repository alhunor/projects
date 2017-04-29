#ifndef _MARKET_H
#define _MARKET_H

#include "ipc.h"

const int INVALID_ORDER=-100;
const int UNCONDITIONAL_ORDER=0;
const int UNCONDITIONALCUT_ORDER=-50;


struct orderT
{
	int timeEntered;
	int timetoCancel; // if an order does not get executed before this moment it will be cancelled
	double level; // level where the order needs to be executed
	int conditional; // order becomes active if the transaction with this id has been executed. -1 means unconditional order
	int sn; // serial number of the order;
	int buySell; // +1 buy, -1 Sell
};

struct confT
{
	int time;
	int sn;
	double level; // XXX should be the level parameter really
	int buySell;
	// add qty traded at some point
};

class orders
{
public:
	orders(); // no default constructor
	orders(int _maxOrders) : nbOrders(0), maxOrders(_maxOrders) { orderList=new orderT[maxOrders];}
	int addOrder(orderT* order);
	bool cancelTimedOut(int currentTime); // cancels timed out orders
	bool cancelConditional(int sn, int currentTime); // cancels conditional orders depending on "sn"
	void executeOrder(double bid, double ask, int currentTime, mqueue<confT>* confirmations); // poat confirmation in queue
	static orderT* newOrder(int buySell, double level, int condition, int timeEntered, int timetoCancel);
	void markConditional(int sn, int condition); // positive numbers mark conditional orders, 
	//NoCondition is an unconditional order that can be executed immediately and INVALIDCondition mean an order that will have to be deleted

	void clear();
	int count(int buySell); // count buy and sell orders

protected:
	int nbOrders, maxOrders;
	orderT *orderList;
	static int sn; // serial number od the order, auto incremented
};


class marketT
{ // hold the order and processes market ticks
public:
	marketT(); // no default constructor
	~marketT() {removeMutex(m);} 
	marketT(int _nbMaxOrders, mqueue<orderT>* _requests, mqueue<confT>* _confirmations) : nbMaxOrders(_nbMaxOrders),
			orders(nbMaxOrders), requests(_requests), confirmations(_confirmations ) {m=createMutex("MutexOrders", false);}
	bool marketTick(double bid, double ask, int currentTime);
	void processRequests();
	void clear() { 	orders.clear(); }

	inline int count(int buySell) {return orders.count(buySell);} // count buy and sell orders

protected:
	int nbMaxOrders;
	MUTEX m; // ptotects manipulation inside orders
	orders orders; // sell and buy orders
	mqueue<orderT>* requests;
	mqueue<confT>* confirmations;
};

#endif