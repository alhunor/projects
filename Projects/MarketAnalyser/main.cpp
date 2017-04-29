#define _CRT_SECURE_NO_WARNINGS
#include <time.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "filesystems.h"


#include <iostream>
#include <fstream>
#include <limits>
#include <math.h>
#include "ipc.h"
#include "market.h"
#include "statistics.h"
#include "strategies.h"
#include "trader.h"
#include "parameters.h"

using namespace std;

double sum[24*60], sum2[24*60];
int nbtrades[24*60];


void heatMap(const char* filename)
{
	binstream in(filename, false);
	dbRecordT dbr;
	struct tm * ptm;
	int i;
	for(i=0; i<24*60; i++)
	{
		sum[i]=0;
		sum2[i]=0;
		nbtrades[i] = 0;
	}

	int minute;
	//for(=0; i<10; i++)
	while (!in.eof())
	{
		in.readblock(&dbr, sizeof(dbr));
		ptm = gmtime ( &dbr.time );
		//year = 1900+ptm->tm_year;
		minute = ptm->tm_hour*60+ptm->tm_min;
		//cout<<minute<<"  "<<dbr.bid<<"  "<<dbr.ask<<endl;
		sum[minute] +=dbr.bid;
		sum2[minute] +=dbr.bid*dbr.bid;
		nbtrades[minute]++;
	}

	for(i=0; i<24*60; i++)
	{
		if (nbtrades[i]>0)
			cout<<i<<","<<(sum2[i]-(sum[i]*sum[i])/nbtrades[i])/(nbtrades[i]-1) <<endl;
	}
} // void heatMap(const char* filename)



/*
TODO:
	- optimize on w1 and w2 
	- optimize on maxOrders, stoploss
	- collect more data with curl and stick them in database
	- start using and experiment with latency
	- implement regression
	- implement a data reader: should recognize current format, connect to a mysql database or read unedited raw data as downloaded from website
	- implement rebase and epoch
	- solve oncurrent access to orders from market and trader. Mutex has been created already, rest is to be done
TEST:
DONE:
	- code up a stop loss , if strategy does not work, sell out current position
	- functionality to overwrite orders tested.
*/

	mqueue<confT> confirmations("confirmations");
	mqueue<orderT> requests("requests");


	int capacities[3]={parameters::CounterLength1, parameters::CounterLength2, parameters::CounterLength3};
	 
	summation<standardAccumulator> data(3, capacities); // size, nbCounters, capacities

	marketT market(parameters::MaxOrders, &requests, &confirmations); // maxorder, latency, requests, confirmations
	 
	trader tra(parameters::ShortLimit, parameters::LongLimit, &requests, &confirmations, &market) ; // shortLimit, longLimit, requests, confirmations
	 
	strategy stra(&tra, &data); // trader, strategy


void analyseData()
{
int nrticks=0; 

	ifstream in;
	int date, time;
	float bid, ask;

	in.open("data.csv");
	if (! in.is_open())
		throw "Failed to oppen file";

//	in.seekg (0, ios::beg);

	confirmations.clear();
	requests.clear();
	data.clear();
	market.clear();
	tra.clear();
	stra.clear();
	nrticks=0;
	
	while (!in.eof())
	{
		in >> date>>time>>bid>>ask;
		nrticks++;
		
		data.addDataPoint(bid);
	//	cout<<nrticks<<" "<<bid<<"  "<<data.mean(0)<<"  "<<data.stdev(0)<<endl;

		stra.executeStrategy(bid, time); // executes strategy and post trading request depending on limits and current position	
		market.processRequests(); // post trading requests

		market.marketTick(bid, ask, time);	// executes orders and posts confirmations

		tra.stopLoss(bid, ask, time);

		tra.processConfirmations(); // processes confirmations to track pnl and currentPosition
	}
	in.close();

	cout<<stra.w1<<" , "<<stra.w2<<" , "<<tra.nbRequests<<" , ";
	tra.reportPnL(bid, ask);
}


void main(int argc, char** argv)
{
	char* filename ="F:\\cobra\\unique\\2005\\EUR-USD.db";

	memoryMappeddbTable mmdbt;
	dbRecordT* rec = (dbRecordT*)mmdbt.map(filename);
	if (!rec)
	{
		cout<<"Error opening file"<<endl;
		return;
	}
	int nbRecords =mmdbt.nFileSize / sizeof(dbRecordT);

	index* counters[4];
	counters[0] = new ema(0.1);
	counters[1] = new mean(30); //higher alpha tracks the data more closely
	counters[2] = new stdev(30, counters[1]);
	counters[3] = new emadev(0.1, counters[0]);

	summation2 sa(4,counters);
	const int hi=503;
	const int lo=2;
	ofstream out("d.csv");

	for (int i=lo; i<hi; i++)
	{
		sa.addDataPoint(rec[i].bid);
		sa.visualise(out, rec, i, lo);
	}

	delete counters[0];
	delete counters[2];
	delete counters[1];

	mmdbt.unmap();

	cout.setf( ios_base::fixed, ios_base::floatfield );
    cout.precision( 3 );

	stra.setweights(parameters::w1, parameters::w2);

	analyseData();

	cout<<"Done. Press any key to continue.."<<endl;
	//char c;
	//cin>>c;
} // void main(int argc, char** argv)