#include <iostream>
#include "ipc.h"
#include "market.h"
#include "trader.h"
#include "parameters.h"

using namespace std;

extern int nrticks;

trader::trader (int _shortLimit, int _longLimit, mqueue<orderT>* _requests, mqueue<confT>* _confirmations, marketT* _market)  : pnl(0),
	shortLimit(_shortLimit), longLimit(_longLimit), initialised (false), currentPosition(0), requests(_requests), confirmations(_confirmations), market(_market)
{
	init();
}

void trader::init()
{
	initialised = true;
	watchlist = new positionT[2*max(-shortLimit, longLimit)];
	clear();
}

void trader::reportPnL(double bid, double ask)
{
	if (currentPosition<0)
		pnl+=currentPosition*ask;
	if (currentPosition>0)
		pnl+=currentPosition*bid;
	cout<<pnl<<endl;
#ifdef DEBUG
	cout<<"Netted "<< currentPosition<<endl;
#endif
}

int trader::requestTransaction(int buySell, double level, int conditional, int timeEntered, int timetoCancel)
{
	if (!initialised)
		throw "Error";

	nbRequests++;

	switch (buySell)
	{
		case -1: //sell
			if (buySell+currentPosition+market->count(-1)<shortLimit)
				return false;
			break;
		case +1: //buy
			if (buySell+currentPosition+market->count(+1)>longLimit)
				return false;
			break;
		default:
			throw "Error";
	}
// post the order in the queue
	orderT* newOrder = orders::newOrder(buySell, level, conditional, timeEntered, timetoCancel);
	requests->push(newOrder);
	return newOrder->sn;
} // int trader::requestTransaction(int buySell, double level, int conditional, int timeEntered, int timetoCancel)


void trader::processConfirmations()
{
	confT* conf=confirmations->pop_front();
	int i, k;
	int pos;
	
	while (conf!=NULL)
	{
		switch (conf->buySell)
		{
			case -1:
			case +1:
				pnl-=conf->buySell*conf->level;
				#ifdef DEBUG
				cout<<nrticks<< ": Executed "<<conf->buySell<<" "<<conf->level<<endl;
				#endif

				k=abs(currentPosition); // // current position without the latest confirmation
				currentPosition+=conf->buySell; // current position including the latest confirmation

				if (k<abs(currentPosition+conf->buySell))
				{
					// increased long or short position
					watchlist[nbWatched].level=conf->level;
					watchlist[nbWatched].longShort = conf->buySell; //negative level indicate a short position
					watchlist[nbWatched].time=conf->time;
					watchlist[nbWatched].sn=conf->sn;				
					nbWatched++;
				} else
				{ // one position has become empty, reorganise data
				// the lowest level order has been executed always, find its position
					pos=0;
					for (i=1; i<nbWatched; i++)
						if (watchlist[i].level*watchlist[i].longShort<watchlist[pos].level*watchlist[i].longShort)
							pos=i;
					for (i=pos; i<nbWatched-1; i++)
					{						
						watchlist[i]=watchlist[i+1];
					}
					if (nbWatched>0) nbWatched--;
				}
				break;
			default:
				throw "Umknown order code.";
		};
		delete conf; // has been allocated in orders::executeOrder(..)
		conf=confirmations->pop_front(); // process next confirmation, if any
	}
} // void trader::processConfirmations()


void trader::stopLoss(double bid, double ask, int currentTime)
{
	int i;
	int shift=0;
	double pnl;

	for (i=0; i<nbWatched; i++)
	{						
		if (shift>0)
			watchlist[i-shift]=watchlist[i];

		switch (watchlist[i].longShort)
		{
			case +1:
				pnl = watchlist[i].level-ask;
				break;
			case -1:
				pnl = bid-watchlist[i].level;
				break;
			default:
				throw "Invalid order";
		}

		if (pnl<parameters::positionStopLoss) //positionStopLoss is a negative value
		{
			shift++;
			//post order to cut position
			requestTransaction(-watchlist[i].longShort,0,UNCONDITIONALCUT_ORDER, currentTime);
		}
		// once an order has been sent to cut a position,it is removed from traders watchlist;
	}
	nbWatched -= shift;		
}
