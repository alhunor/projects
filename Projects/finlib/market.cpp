#include <iostream>
#include "ipc.h"
#include "market.h"
#include "parameters.h"

int orders::sn=1;


using namespace std;

void historyT::addd(int tick, double order)
{
	HistoricRecordT r;
	r.tick=tick;
	r.order = order;
	q.push_back(r);
	n++;
}




int orders::addOrder(orderT* order)
{
	orderList.push_back(*order);
	if (order->qty>0 && order->level>buyo) buyo=order->level;
	if (order->qty<0 && order->level<sello) sello=order->level;

	history.addd(order->tickEntered, order->level*order->qty);
	return order->sn;
}

void orders::sendCancelConfirmation(int sn)
{
	// send confirmation to trader
	confT* conf = new confT;
	conf->cancelled=true;
	conf->sn = sn;
	//conf->time=currentTime;
	//conf->tick=tick;
	//conf->level=level;
	//conf->qty=it->qty;
	confirmations->push(conf);
}


void orders::cancelTimedOut(int currentTime)
{
	bool cancelled = false;
	list<orderT>::iterator it;
	for(it=orderList.begin(); it!=orderList.end(); ++it)
	{
		if (it->lifeSpan<currentTime)
		{
			// send cancel confirmation to trader
			sendCancelConfirmation(it->sn);
			orderList.erase(it);
			cancelled  =true;
		}	
	}
	if (cancelled) recomputeMinMax();
} // orders::cancelTimedOut(int currentTime)


void orders::executeOrder(double bid, double ask, int tick, int currentTime, mqueue<confT>* confirmations)
{
	bool insideLimits=true; // should become multithreaded complex function
	double level;
	bool traded = false;
	if (buyo<bid && sello>ask) return;

	list<orderT>::iterator it=orderList.begin(); 

	while (it!=orderList.end())
	{
		level=it->qty>0?ask:bid;

		if (it->orderType==ATMARKET || (it->level*it->qty > level*it->qty && it->timeEntered + parameters::Latency < currentTime && insideLimits) )
		{
			confT* conf=new confT(); // allocated memory is freed up trader::processConfirmations()
			conf->time=currentTime;
			conf->cancelled=false;
			conf->tick=tick;
			conf->sn=it->sn;
			conf->level=level;
			conf->qty=it->qty;
			confirmations->push(conf);
			it = orderList.erase(it);
			traded= true;
		} else
		{
			++it;
		}
	} // while (it!=orderList.end())
	if (traded)
		recomputeMinMax();
} // void orders::executeOrder( .. )


orderT* orders::newOrder(double qty, double level, orderTypeT orderType, int tickEntered, int timeEntered, int lifeSpan)
{
	orderT* newo= new orderT(); // allocated memory is freed by market::processRequests()
	newo->level=level;
	newo->orderType=orderType;
	newo->tickEntered=tickEntered;
	newo->timeEntered=timeEntered;
	newo->lifeSpan=lifeSpan;
	newo->qty=qty;
	newo->sn=sn++;
	return newo;
} // orderT* orders::newOrder(..)

void orders::clear()
{
	sn = 1;
	history.clear();
	orderList.clear();
	sello = DBL_MAX; 
	buyo = - DBL_MAX;
}


void orders::recomputeMinMax()
{
	buyo = -DBL_MAX;
	sello = DBL_MAX;
	for (list<orderT>::iterator it = orderList.begin(); it!=orderList.end(); ++it)
	{
		if (it->qty>0 && it->level>buyo) buyo = it->level;
		if (it->qty<0 && it->level<sello) sello = it->level;
	}
} // void orders::recomputeMinMax()


void orders::cancel(orderT* order)
{
	bool cancelled = false;
	list<orderT>::iterator it = orderList.begin();
	while( it!=orderList.end() )
	{
		if ( 
			(order->qty<0 && it->qty>0 && it->level>order->level) // Cancel all buy orders below newo->level
			||
			(order->qty>0 && it->qty<0 && it->level<order->level) // Cancel all sell orders above newo->level
			)
		{
			sendCancelConfirmation(it->sn);
			it = orderList.erase(it);
			if (order->qty>0 && order->level>buyo) cancelled = true;
			if (order->qty<0 && order->level<sello) cancelled = true;
		} else
		{
			++it;
		}
	} // while( it!=orderList.end() )
	if (cancelled)
		recomputeMinMax();
} // void orders::cancel(orderT* newo)


bool marketT::marketTick(double bid, double ask, int currentTime, int tick)
{	

	orders.executeOrder(bid, ask, tick, currentTime, confirmations); // buy at the ask
	return true;
}





void marketT::processRequests()
{	// will process all the requests from the queue
	orderT* newo=requests->pop_front(); // has been allocated in orders::newOrder(..)

	while (newo!=NULL)
	{
		if (newo->orderType==CANCEL)
		{
			orders.cancel(newo);
		} else
		{
			orders.addOrder(newo);
		}
		delete newo;
		newo=requests->pop_front(); // get next request
	} // while (newo!=NULL)
} // void marketT::processRequests()

