#define _CRT_SECURE_NO_WARNINGS
#include "backTesting.h"
#include "myStuff/FileSystems.h"
#include "ExcelConversions.h"

#include "myStuff/hftimer.h"
#include "myStuff/dates.h"
#include "ipc.h"
#include "market.h"
#include "statistics.h"
#include "strategies.h"
#include "trader.h"
#include "timeseries.h"


/*
A	Boolean	short (0=false or 1=true)
B	double	
C, F	char *	Null-terminated ASCII byte string.
D, G	unsigned char *	Length -counted ASCII byte string.
H	unsigned short [int]	WORD
I	[signed] short [int]	16-bit
J	[signed long] int	32-bit
K	FP	Floating-point array structure.
R	XLOPER	Values, arrays, and range references.
*/

inline double square(double x) {return x*x;}


mqueue<confT> confirmations("confirmations");
mqueue<orderT> requests("requests");


#include <xlw\xlw.h>

/* TODO
- introduce order type : at market level
- introduce ability to trader fractions of the underlying
- implement crossing of the moments strategy
- implement a multivariable optimiser
- review logic for cancelling older orders as new orders come in
- update strategy
*/

// Force export of functions implemented in XlOpenClose.h and required by Excel
#pragma comment (linker, "/export:_xlAutoOpen")
#pragma comment (linker, "/export:_xlAutoClose")

using namespace xlw;


/////////////   TimeSeriesFromFile      ///////////////////////////////////////////////


xlw::XlfOper listRecFile(TimeSeriesFromFile<timeStampedBidAskT>& ts, int minPos, int maxPos, int step)
{
	int n = (maxPos-minPos +step -1) / step;
	MyMatrix ret(n+1, 3);
	int i, pos;
	pos=0;
	for (i=minPos; i<maxPos; i+=step)
	{
		ret(pos+1,0)=(double)ts.rec[i].time;
		ret(pos+1,1)=ts.rec[i].bid;
		ret(pos+1,2)=ts.rec[i].ask;
		pos++;
	}
	XlfOper xlo = XlfOper(ret);
	xlo.SetElement(0,0, "Time");
	xlo.SetElement(0,1, "Bid");
	xlo.SetElement(0,2, "Ask");
	return xlo;
} // xlw::XlfOper list(TimeSeriesFromFile<timeStampedBidAskT>& ts, int minPos, int maxPos, int step)


xlw::XlfOper listTextFile(TextFile& tf, int minPos, int maxPos, int step)
{
	if (minPos<0 || maxPos>tf.nbLines())
	{
		throw "Line numbers are out of range.";
	}
	int n = (maxPos-minPos +step -1) / step;
	const char* s;

	XlfOper xlo(n,1);

	for (int i=minPos; i<maxPos; i+=step)
	{
		s = tf.getLine(i);
		xlo.SetElement(i-minPos, 0, s);
	}
	return xlo;
} // xlw::XlfOper list(TextFile& tf, int minPos, int maxPos, int step)






// Data series database https://datamarket.com/data/list/?q=cat:ecc%20provider:tsdl

//https://www.worldtradingdata.com/documentation
//https://iextrading.com/apps/stocks/


extern "C" LPXLFOPER __declspec(dllexport) xlTimeSeries(XlfOper Command, XlfOper Arg)
// returns handle to timeseries object in dbRecordT[] format.
{
	EXCEL_BEGIN;

	// Checks if called from the function wizard
	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

	hftimer timer;
	timer.startTimer();
	int elapsed;

	TimeSeriesFromFile<timeStampedBidAskT>* ts;
	TextFile* tf;
	int h;

	char* command = Command.AsString();
	if (_stricmp(command, "mapTimeStampedBidAsk")==0)
	{
		//char* filename = "F:\\cobra\\unique\\2005\\EUR-USD.db";
		char* filename = Arg.AsString();
		ts = new TimeSeriesFromFile<timeStampedBidAskT>(filename);
		if (ts==NULL)
			return XlfOper("File not found.");

		int h = handle.add(ts);
		return XlfOper(double(h));
	} // if (_stricmp(command, "mapTimeStampedBidAsk")==0)

	if (_stricmp(command, "mapTextFile")==0)
	{
		//char* filename = "F:\\Projects\\finlib\\ZAZA.csv";
		char* filename = Arg.AsString();
		tf = new TextFile(filename);
		
		if (tf==NULL)
			return XlfOper("File not found.");

		int nl = tf->nbLines();

		int h = handle.add(tf);
		return XlfOper(double(h));
	} // if (_stricmp(command, "mapTextFile")==0)

	if (_stricmp(command, "openCSVFile")==0)
	{
//		Format of the file must be: 
//			First line is a header with column napes Date, Open, High, Low, Close, Volume -- ignored
//			Each of the following lines contains a sextuple (DOHLCV)

		char* filename = Arg.AsString();

		Descriptor d;
		d.hasHeader=true;
		d.nbFields=6;
		d.recordSize=sizeof(DOHLCV);

		bool b = openCSVFile(filename, ',', &d, DOHLCVparser);

/*
		binstream bin(filename, false);
		char buff[64];
		bin.getline(buff,50);
		bin.getline(buff,50);
		bin.getline(buff,50);
		bin.getline(buff,50);
		bin.getline(buff,50);
		int days;
		while (!bin.eof())
		{
			bin.getToken(buff,10,',', false);
			if (strlen(buff)==9)
			{
				days = dateFromString(buff);
			}
			h++;
		}
*/		

		return XlfOper(0.0);
	} // if (_stricmp(command, "mapTextFile")==0)

	if (_stricmp(command, "listCommands")==0)
	{
		labelValue ret;
		ret.add("mapTimeStampedBidAsk", "{filename}");
		ret.add("mapTextFile", "{filename}");
		ret.add("info", "{handle}");
		ret.add("list", "labelValue{handle, minPos, maxPos, step}");
		ret.add("createIndicator", "labelValue{name, ..}");
		ret.add("analyse", "labelValue{handle, minPos, maxPos, nbIndicators, indicator1, indicator2, ..}");
		ret.add("simulate", "labelValue{..}");
		ret.add("createTrader", "labelValue{..}");
		ret.add("createMarket", "labelValue{..}");
		ret.add("createStrategy", "labelValue{..}");
		ret.add("remove", "{handle}");
		ret.add("clearAll", "");

		return labelValuetoXlfOper(ret);
	} // if (_stricmp(command, "listCommands")==0)

	if (_stricmp(command, "info")==0)
	{
		h = Arg.AsInt();
		ts = dynamic_cast<TimeSeriesFromFile<timeStampedBidAskT>*>((MotherOfAllBaseObjects* )handle[h]);
		if (ts)
		{
			labelValue ret;
			ret.add("Filename", ts->filename);
			ret.add("NbRecords", ts->nbRecords);
			ret.add("MinTime", (double)ts->rec[0].time);
			ret.add("MaxTime", (double)ts->rec[ts->nbRecords-1].time);
			return labelValuetoXlfOper(ret);
		}

		tf = dynamic_cast<TextFile*>((MotherOfAllBaseObjects* )handle[h]);
		if (tf)
		{
			labelValue ret;
			ret.add("Filename", tf->filename);
			ret.add("nbLines", tf->nbLines());
			ret.add("FileSize(bytes)", tf->fileSize());
			return labelValuetoXlfOper(ret);
		}

		throw "Handle does not point to <TimeSeriesFromFile> or <TextFile> type.";

	} // if (_stricmp(command, "info")==0)


	if (_stricmp(command, "statistics")==0)
	{
		h = Arg.AsInt();
		ts = dynamic_cast<TimeSeriesFromFile<timeStampedBidAskT>*>((MotherOfAllBaseObjects* )handle[h]);
		if (!ts)
			throw "Handle does not point to underlying <TimeSeriesFromFile> type.";

		double meanBid=0;
		double meanbidask = 0;
		double stdevBid =0;
		double min = 99999999;
		double max = -min;
		for (int i=0; i<ts->nbRecords; i++)
		{
			meanbidask += ts->rec[i].ask - ts->rec[i].bid;
			meanBid += ts->rec[i].bid;
			if (ts->rec[i].bid<min) min = ts->rec[i].bid;
			if (ts->rec[i].bid>max) max = ts->rec[i].bid;
		}
		meanBid /=  ts->nbRecords;
		for (int i=0; i<ts->nbRecords; i++)
		{
			stdevBid += square(ts->rec[i].bid - meanBid);
		}
		stdevBid /= ts->nbRecords;

		labelValue ret;
		ret.add("NbRecords", ts->nbRecords);
		ret.add("meanBid", meanBid );
		ret.add("stdevBid", stdevBid);
		ret.add("average bid-ask", meanbidask / ts->nbRecords);
		ret.add("min", min);
		ret.add("max", max);

		return labelValuetoXlfOper(ret);
	} // if (_stricmp(command, "statistics")==0)

	if (_stricmp(command, "list")==0)
	{
		labelValue arg;
		labelValueAddXlfOper(arg, Arg);
		int minPos = (int)arg.getNum("minPos");
		int maxPos = (int)arg.getNum("maxPos");
		int step = (int)arg.getNum("step");		

		ts = dynamic_cast<TimeSeriesFromFile<timeStampedBidAskT>*>((MotherOfAllBaseObjects* )arg.getObj("fileHandle"));
		if (ts)
		{			
			return listRecFile(*ts, minPos, maxPos, step);
		}

		tf = dynamic_cast<TextFile*>((MotherOfAllBaseObjects* )arg.getObj("fileHandle"));
		if (tf)
		{			
			return listTextFile(*tf, minPos, maxPos, step);
		}

		throw "Handle does not point to underlying <TimeSeriesFromFile> type.";
	} // if (_stricmp(command, "list")==0)

	if (_stricmp(command, "createIndicator")==0)
	{
		labelValue arg;
		labelValueAddXlfOper(arg, Arg);
		indicatorT* indi = NULL;
		h = (int)arg.getHandle("handle");
		if (h>-1) // I think the functionality to update indicators is useless
		{
			indi = dynamic_cast<indicatorT*>((MotherOfAllBaseObjects* )handle[h]);
		}
		if (indi)
		{
			indi->update(arg);
		} else
		{
			indi = createIndicator(arg);	
			if (indi==NULL)
				return XlfOper("Unknown indicator name.");
			h = handle.add(indi);
		}		
		
		// return handle to indicator
		return XlfOper((double)h);
	} // if (_stricmp(command, "createIndicator")==0)

	if (_stricmp(command, "createMarket")==0)
	{
		labelValue arg;
		labelValueAddXlfOper(arg, Arg);
		int maxOrders = (int)arg.getNum("maxOrders");
		marketT* market = new marketT(maxOrders, &requests, &confirmations); 
		h = handle.add(market);
		return XlfOper((double)h);
	} // if (_stricmp(command, "createMarket")==0)

	if (_stricmp(command, "createTrader")==0)
	{
		labelValue arg;
		labelValueAddXlfOper(arg, Arg);
		int shortLimit = (int)arg.getNum("shortLimit");
		int longLimit = (int)arg.getNum("longLimit");
		marketT* market = dynamic_cast<marketT*>((MotherOfAllBaseObjects* )arg.getObj("market"));
		if (!market)
			throw "Handle does not point to underlying <marketT> type.";

		traderT* trader = new traderT(shortLimit, longLimit, &requests, &confirmations, market); 
		h = handle.add(trader);
		return XlfOper((double)h);
	} // if (_stricmp(command, "createTrader")==0)
 
	if (_stricmp(command, "createStrategy")==0)
	{
		labelValue arg;
		labelValueAddXlfOper(arg, Arg);
		traderT* trader = dynamic_cast<traderT*>((MotherOfAllBaseObjects* )arg.getObj("trader"));
		if (!trader)
			throw "Handle does not point to underlying <traderT> type.";

		strategyT* strategy = createStrategy(trader, arg);
		h = handle.add(strategy);
		return XlfOper((double)h);
	} // if (_stricmp(command, "createTrader")==0)

	if (_stricmp(command, "analyse")==0)
	{
		labelValue arg;
		labelValueAddXlfOper(arg, Arg);

		ts = dynamic_cast<TimeSeriesFromFile<timeStampedBidAskT>*>((MotherOfAllBaseObjects* )arg.getObj("fileHandle"));
		if (!ts)
			throw "Handle does not point to underlying <TimeSeriesFromFile> type.";

		int minPos = (int)arg.getNum("minPos");
		int maxPos = (int)arg.getNum("maxPos");
		summation2* sa = createSummation(arg);
		int j;
		double v;
		MyMatrix ret(maxPos-minPos+2, sa->nbIndicators+3);

		for (int i=minPos; i<maxPos; i++)
		{
			sa->addDataPoint((int)ts->rec[i].time, ts->rec[i].bid, ts->rec[i].ask);

			ret(i-minPos+1,0)=(double)(ts->rec[i].time-ts->rec[minPos].time);
			ret(i-minPos+1,1)=ts->rec[i].bid;
			ret(i-minPos+1,2)=ts->rec[i].ask;
			for (j=0; j<sa->nbIndicators; j++)
			{
				v = sa->value(j);
				ret(i-minPos+1,3+j)=v;
			}
		}
		XlfOper xlo = XlfOper(ret);
		xlo.SetElement(0,0, "Time");
		xlo.SetElement(0,1, "Bid");
		xlo.SetElement(0,2, "Ask");
		for (j=0; j<sa->nbIndicators; j++)
		{
			xlo.SetElement(0,3+j, sa->name(j));
		}
		delete sa;

		elapsed = timer.getTime();
		return xlo;
	} // if (_stricmp(command, "analyse")==0)

	if (_stricmp(command, "simulate")==0)
	{
		labelValue arg;
		labelValueAddXlfOper(arg, Arg);
		marketT* market = dynamic_cast<marketT*>((MotherOfAllBaseObjects* )arg.getObj("market"));
		if (!market)
			throw "Handle does not point to underlying <marketT> type.";
		traderT* trader = dynamic_cast<traderT*>((MotherOfAllBaseObjects* )arg.getObj("trader"));	
		if (!trader)
			throw "Handle does not point to underlying <traderT> type.";
		strategyT* stra = dynamic_cast<strategyT*>((MotherOfAllBaseObjects* )arg.getObj("strategy"));
		if (!stra)
			throw "Handle does not point to underlying <strategyT> type.";
		stra->update(arg);
		
		ts = dynamic_cast<TimeSeriesFromFile<timeStampedBidAskT>*>((MotherOfAllBaseObjects* )arg.getObj("fileHandle"));
		if (!ts)
			throw "Handle does not point to underlying <TimeSeriesFromFile> type.";

		int minPos = (int)arg.getNum("minPos");
		int maxPos = (int)arg.getNum("maxPos");

		double pnl = simulate(ts, stra, market, trader, minPos, maxPos);

		elapsed = timer.getTime();
		return XlfOper(pnl);
	} // if (_stricmp(command, "simulate")==0)
	if (_stricmp(command, "report")==0)
	{
		labelValue arg; 
		labelValueAddXlfOper(arg, Arg);

		traderT* trader = dynamic_cast<traderT*>((MotherOfAllBaseObjects* )arg.getObj("trader"));	
		if (!trader)
			throw "Handle does not point to underlying <traderT> type.";

		XlfOper ret = toXlfOper(trader->tradingHistory);
		return ret;
	} // 	if (_stricmp(command, "report")==0)

	if (_stricmp(command, "remove")==0)
	{	// can free all handle types
		h = Arg.AsInt();
		MotherOfAllBaseObjects* base = (MotherOfAllBaseObjects* )handle[h];
		delete base;
		handle.remove(h);
		return XlfOper("Handle has been freed.");
	} // if (_stricmp(command, "remove")==0)

	if (_stricmp(command, "clearAll")==0)
	{	// reoves all handles
		MotherOfAllBaseObjects* base;
		for (int h=0; h<handle.nbHandles() ; ++h)
		{
			base = (MotherOfAllBaseObjects* )handle[h];
			delete base;
		}
		handle.clear();
		return XlfOper("All handles have been freed.");
	} // if (_stricmp(command, "remove")==0)

	return XlfOper("Unknown command.");
	EXCEL_END;
} //  xlmapTimeSeries(XlfOper Filename)



namespace
{
//TimeSeries & Trader
	XLRegistration::Arg TimeSeriesArgs[] = {
       { "Command", "Command", "XLF_OPER" },
       { "Arguments", "Arguments", "XLF_OPER" }
    };
    XLRegistration::XLFunctionRegistrationHelper registerMapTimeSeries
	(   "xlTimeSeries", "TimeSeries", "Operate on a time series.",
        "FinLib", TimeSeriesArgs, 2);

}
