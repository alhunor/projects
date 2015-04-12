#include <iostream>
#include "ipc.h"
#include "market.h"
#include "trader.h"
#include "parameters.h"


using namespace std;

extern int nrticks;

traderT::traderT (int _shortLimit, int _longLimit, mqueue<orderT>* _requests, mqueue<confT>* _confirmations, marketT* _market)  : pnl(0),
	shortLimit(_shortLimit), longLimit(_longLimit), initialised (false), currentPosition(0), requests(_requests), confirmations(_confirmations), market(_market)
{
	init();
}

void traderT::init()
{
	initialised = true;
	clear();
}

double traderT::reportPnL(double bid, double ask)
{
	if (currentPosition<0)
		pnl+=currentPosition*ask;
	if (currentPosition>0)
		pnl+=currentPosition*bid;
	return pnl;
}



int traderT::requestTransaction(double qty, double level, orderTypeT orderType, int tickEntered, int timeEntered, int lifeSpan)
{
	if (!initialised)
		throw "Error";

	switch (sgn(qty))
	{
		case -1: //sell
			if (qty+currentPosition+pendingShortOrders<shortLimit)
				return false;
			break;
		case +1: //buy
			if (qty+currentPosition+pendingLongOrders>longLimit)
				return false;
			break;
		default:
			throw "Error";
	}
	// check in the pending orders that we can initiate this transaction

	orderT* newOrder;

	for (list<orderT>::iterator it = pendingOrders.begin(); it!=pendingOrders.end() ; ++it)
	{
		if ( 
			(qty<0 && it->qty>0 && it->level>level) // Cancel all buy orders below level
			||
			(qty>0 && it->qty<0 && it->level<level) // Cancel all sell orders above level
			)
		{
			newOrder = orders::newOrder(qty, level, CANCEL, tickEntered, timeEntered, lifeSpan);
			requests->push(newOrder);
			// and do not execute this one
			return 0;		
		}
	}

	
	// post the order in the queue
	newOrder = orders::newOrder(qty, level, orderType, tickEntered, timeEntered, lifeSpan);
	pendingOrders.push_back(*newOrder);
	if (qty<0)
		pendingShortOrders +=qty; 
	if (qty>0)
		pendingLongOrders +=qty;

	requests->push(newOrder);
	return newOrder->sn;
} // int trader::requestTransaction(int buySell, double level, int conditional, int timeEntered, int timetoCancel)

list<confT>::iterator traderT::minLongPosition()
{
	if (positions.empty()) return positions.end();

	list<confT>::iterator min = positions.begin();
	while (min!=positions.end() && min->qty<0)
	{
		++min;
	}

	if (min==positions.end())
		return positions.end();
	
	for (list<confT>::iterator it = min; it!=positions.end(); ++it)
	{
		if (it->qty>0 && it->level < min->level) 
		{
			min = it;
		}
	}
	return min;
} // list<confT>::iterator traderT::minLongPosition()

list<confT>::iterator traderT::maxShortPosition()
{
	if (positions.empty()) return positions.end();

	list<confT>::iterator max = positions.begin();
	while (max!=positions.end() && max->qty>0)
	{
		++max;
	}

	if (max==positions.end())
		return positions.end();
	
	for (list<confT>::iterator it = max; it!=positions.end(); ++it)
	{
		if (it->qty<0 && it->level > max->level) 
		{
			max = it;
		}
	}

	return max;
} // list<confT>::iterator traderT::maxShortPosition()


void traderT::processConfirmations()
{
	confT* conf=confirmations->pop_front();
	list<confT>::iterator it;
	list<orderT>::iterator ito;

	double qty;
	while (conf!=NULL)
	{
		for (ito = pendingOrders.begin(); ito!=pendingOrders.end() && ito->sn!=conf->sn; ++ito);
		if (ito->sn!=conf->sn)
		{
			// this should never happen, received confirmation for an order that has not been initiated 		
			throw "Unknown order has been cancelled.";
		}
		qty = conf->qty;
		if (qty<0)
		{ // sell
			pendingShortOrders -= ito->qty; 

		} else
		{ // buy
			pendingLongOrders -= ito->qty;
		}

		if (! conf->cancelled)
		{
			tradingHistory.addd(conf->tick, qty*conf->level);
			pnl -= qty * conf->level;
			currentPosition+=qty; // current position including the latest confirmation

			if (qty<0)
			{ // sell
				it = minLongPosition();
			} else
			{ // buy
				it = maxShortPosition();
			}
			if (it==positions.end()) 
			{
				positions.push_back(*conf);
			} else
			{
				if (qty+it->qty==0.0)
				{
					// cancel long and short position
					positions.erase(it);
				} else
				{
					conf->qty += it->qty;
					*it = *conf;
				}
			}
			pendingOrders.erase(ito);

		} // if (! conf->cancelled)
		delete conf; // has been allocated in orders::executeOrder(..)
		conf=confirmations->pop_front(); // process next confirmation, if any
	} // while (conf!=NULL)
} // void trader::processConfirmations()


void traderT::stopLoss(double bid, double ask, int currentTime, int tick)
{
	double pnl=0;
	double qty;

	for (list<confT>::const_iterator it = positions.begin(); it!=positions.end(); ++it)
	{
		qty = it->qty;
		if (qty<0)
		{
			pnl = qty * (ask-it->level);		
		} else
		{
			pnl = qty * (bid-it->level);
		}
		if (pnl<parameters::positionStopLoss) //positionStopLoss is a negative value
		{
			//post order to cut position
			requestTransaction(-qty, -SGN(qty)*DBL_MAX, ATMARKET, tick, currentTime);
		}
	} // for (list<confT>::const_iterator it = positions.begin(); it!=positions.end(); ++it)
} // void traderT::stopLoss(double bid, double ask, int currentTime)