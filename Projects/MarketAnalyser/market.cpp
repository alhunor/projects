#include <iostream>
#include "ipc.h"
#include "market.h"
#include "parameters.h"

int orders::sn=1;

using namespace std;

extern int nrticks;


int orders::addOrder(orderT* order)
{
	int i, pos;

	#ifdef DEBUG
	cout<<nrticks<<":"<<order->buySell<<" "<<order->level<<endl;
	#endif

	if (nbOrders==maxOrders)
	{
		//cout<<"full"<<endl;
		// carnet is full, check if an order can be replaced
		// for buy orders: the  smallest order level will be replaced if it is smaller than the new level
		// sell orders are replaced if biggest order level is bigger than current level
		pos=0;
		for (i=1; i<nbOrders; i++)
			if (orderList[i].level*order->buySell<orderList[pos].level*order->buySell)
				pos=i;
		if (orderList[pos].level*order->buySell>order->level*order->buySell) // no order to overwrite
			return -1;
		//cout<<"An order has been overwritten"<<endl;
		markConditional(orderList[pos].sn, INVALID_ORDER); // marks
	} else
	{	
		// carnet is not full
		pos = nbOrders;
		nbOrders++;
	}

	orderList[pos].level=order->level;
	orderList[pos].timeEntered=order->timeEntered;
	orderList[pos].timetoCancel=order->timetoCancel;
	orderList[pos].sn=order->sn;
	orderList[pos].buySell=order->buySell;
	orderList[pos].conditional=order->conditional;
// cancel all conditional dependencies	
	int shift=0;
	for (i=0; i<nbOrders; i++)
	{
		if (shift>0)
			orderList[i-shift]=orderList[i];
		if (orderList[i].conditional==INVALID_ORDER)
		{
			shift++;
		}
	}
	nbOrders-=shift;

	return order->sn;
}

bool orders::cancelTimedOut(int currentTime)
{
	int i;
	int shift=0;
	for (i=0; i<nbOrders; i++)
	{
		if (shift>0)
			orderList[i-shift]=orderList[i];
		if (orderList[i].timetoCancel>=currentTime)
			shift++;
	}
	nbOrders-=shift;
	return shift!=0; // returns true if an order has been cancelled
}

void orders::markConditional(int sn, int condition)
{
	// this function is called to mark every order depending on orderNR=sn to either unconditional (when condition=INCONDITIONAL_ORDER) 
	// or ready to be pruned (when condition=INVALID_ORDER)
	for (int i=0; i<nbOrders; i++)
	{
		if (orderList[i].conditional==sn)
			orderList[i].conditional=condition;
	}
}


void orders::executeOrder(double bid, double ask, int currentTime, mqueue<confT>* confirmations)
{
	bool insideLimits=true; // should become multithreaded complex function
	int i; // temp variables
	int shift=0;
	double level;


	for (i=0; i<nbOrders; i++)
	{
		if (shift>0)
			orderList[i-shift]=orderList[i];

		switch (orderList[i].buySell)
		{
			case +1:
				level=ask;
				break;
			case -1:
				level=bid;
				break;
			default:
				throw "Invalid order";
		}

		if (orderList[i].conditional==UNCONDITIONALCUT_ORDER || (orderList[i].level*orderList[i].buySell>level*orderList[i].buySell && orderList[i].conditional==UNCONDITIONAL_ORDER && orderList[i].timeEntered+parameters::Latency<currentTime && insideLimits) )
		{
			shift++;
			confT* conf=new confT(); // allocated memory is freed up trader::processConfirmations()
			conf->time=currentTime;
			conf->sn=orderList[i].sn;
			if (orderList[i].conditional==UNCONDITIONALCUT_ORDER || MARKETLEVELS)
			{
				conf->level=level;
			} else
			{
				conf->level=orderList[i].level;
			}
			conf->buySell=orderList[i].buySell;
			confirmations->push(conf);
			markConditional(orderList[i].sn, UNCONDITIONAL_ORDER); // XX this needs to be executed on both borders and sorders.
		}
	}
	nbOrders-=shift;
}


orderT* orders::newOrder(int buySell, double level, int conditional, int timeEntered, int timetoCancel)
{
	orderT* newo= new orderT(); // allocated memory is freed by market::processRequests()
	newo->level=level;
	newo->timeEntered=timeEntered;
	newo->timetoCancel=timetoCancel;
	newo->conditional=conditional;
	newo->buySell=buySell;
	newo->sn=sn++;
	return newo;
}

void orders::clear()
{
	nbOrders=0;
	sn = 1;
	// since order list is stored statically it does not need to be freed up
}


int orders::count(int buySell) // count buy or sell orders
{
	int i;
	int q=0;
	for(i=0; i<nbOrders; i++) 
		if (orderList[i].buySell==buySell) q++;
	return q;
}


bool marketT::marketTick(double bid, double ask, int currentTime)
{	
	
	if (nrticks % 200 == 0)
	{
	//	borders.cancelOldOrders(currentTime);
	//	sorders.cancelOldOrders(currentTime);
	}
	orders.executeOrder(bid, ask, currentTime, confirmations); // buy at the ask
	return true;
}

void marketT::processRequests()
{	// will process all the requests from the queue
	orderT* newo=requests->pop_front(); // has been allocated in orders::newOrder(..)
	while (newo!=NULL)
	{
		switch (newo->buySell)
		{
			case -1:
			case +1:
				orders.addOrder(newo);
				break;
			default:
				throw "Umknown order code.";
		};
		delete newo;
		newo=requests->pop_front(); // get next request
	}
}