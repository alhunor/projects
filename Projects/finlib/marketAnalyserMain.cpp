#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <time.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "myStuff/filesystems.h"
#include "myStuff/streams.h"


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
	timeStampedBidAskT dbr;
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
	- optimize on buyorders, stoploss
	- start using and experiment with latency
	- implement regression
	- implement rebase and epoch
	- solve oncurrent access to orders from market and trader. Mutex has been created already, rest is to be done
TEST:
DONE:
*/

