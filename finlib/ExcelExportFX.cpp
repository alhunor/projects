#define _CRT_SECURE_NO_WARNINGS
#include "BlackScholes.h"
#include "ExcelConversions.h"

//#include "parameters.h"
#include "marketData.h"
#include "yieldCurve.h"


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

#include "xlw/xlw.h"


// Force export of functions implemented in XlOpenClose.h and required by Excel
#pragma comment (linker, "/export:_xlAutoOpen")
#pragma comment (linker, "/export:_xlAutoClose")

using namespace xlw;
using namespace std;

extern "C" LPXLFOPER __declspec(dllexport) xlOptionPricer(double forward, double strike, double atmVol, double time, XlfOper ModelDescriptor)
{
	typedef enum {BlackScholes=0, NormalBlackScholes, Hagan} modelType;
	EXCEL_BEGIN;

	// Checks if called from the function wizard
	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

	labelValue lv;
	if (!ModelDescriptor.IsMissing())
		labelValueAddXlfOper(lv,ModelDescriptor);

	modelType model;
	if (lv.getHandle("Model")==-1) 
	{
		model = BlackScholes;
	} else
	{
		string modelName = lv.getStr("Model");
		if (modelName == "BlackScholes")
		{
			model = BlackScholes;
		} else if (modelName == "NormalBlackScholes")
		{
			model = NormalBlackScholes;
		} else if (modelName == "Hagan")
		{
			model = Hagan;
		}
	}
	double ret;

	switch (model)
	{
		case BlackScholes:
			ret = black(forward, strike, atmVol, time);
			break;
		case NormalBlackScholes:
			ret = normalblack(forward, strike, atmVol, time);
			break;
		case Hagan:
			{
			double sigma0 = lv.getNum("sigma0");
			double volvol = lv.getNum("volvol");
			double rho = lv.getNum("rho");
			double beta = lv.getNum("beta");

			ret = hagan(forward, strike, sigma0, volvol, rho, beta, time);
			}
			break;
		default:
			ret = black(forward, strike, atmVol, time);
	}

	return XlfOper(ret);
	EXCEL_END;
}



extern "C" LPXLFOPER __declspec(dllexport) xlBlackScholes(double forward, double strike, double atmVol, double time)
{
	EXCEL_BEGIN;

	// Checks if called from the function wizard
	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

	double ret = black(forward, strike, atmVol, time);

	return XlfOper(ret);
	EXCEL_END;
}




class Dynamics
{
public:
	Dynamics();
	Dynamics(double _f) : f(_f) {}
	virtual MyArray simulate(int n, double timesteps)=0;
protected:
	double f; // forward at time 0.
};

class LognormalDynamics : public Dynamics
{
public:
	LognormalDynamics();
	LognormalDynamics(double f, double _vol) : Dynamics(f), vol(_vol) {}
	virtual MyArray simulate(int n, double timesteps);
protected:
	double vol;
};



MyArray LognormalDynamics::simulate(int n, double dt)
{
	MyArray ma(n);
	double prev = f;
	double sigma2t= 0.5 *vol*vol* dt;
	double sqrtdt =sqrt(dt);
	double dw = .1;
	for (int i=0; i<n; ++i)
	{
		//ma(i)= prev;
		//dw = ran()*sqrtdt;
		prev *= exp(vol*dw-sigma2t);
	
	}
	//ma(n-1) = prev;
	return ma;
}

Dynamics* createDynamics(labelValue& arg)
{
	const char* dynamics = arg.getStr("dynamics").c_str();
	double f = arg.getNum("forward");
	Dynamics* dyn;

	if (_stricmp(dynamics, "LogNormal")==0)
	{
		double vol = arg.getNum("vol");
		dyn = new LognormalDynamics(f, vol);
		return dyn;
	}
	return NULL;
}






extern "C" LPXLFOPER __declspec(dllexport) xlSimulatePath(XlfOper Command, XlfOper Arg)
// returns handle to timeseries object in dbRecordT[] format.
{
	EXCEL_BEGIN;

	// Checks if called from the function wizard
	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);


	labelValue arg;
	labelValueAddXlfOper(arg, Command);
	const char* command = arg.getStr("command").c_str();
	if (_stricmp(command, "SimulateDynamics")==0)
	{
		int n = (int)arg.getNum("nbPoints");
		double pointsPerYear = arg.getNum("pointsPerYear");
		Dynamics* dyn = createDynamics(arg);
		if (!dyn) return XlfOper("Unknown dynamics.");

		dyn->simulate(n, 1/pointsPerYear);
	}

	return XlfOper(1.0);
	EXCEL_END;
}


Currency readycCurveCurrency(XlfOper Curves, int offset)
{
	char* s = Curves(0, offset*2).AsString();
	Currency ccy = toCurrency(s);
	return ccy;
}

yieldCurve* readycCurve(XlfOper Curves, int offset, int marketDate)
{
	std::string convention = Curves(0, offset*2+1).AsString();
	if (convention != "DFs") return NULL;
	yieldCurve* yc = new yieldCurve(marketDate);
	int nbRows = Curves.rows();
	int date;
	float df;
	std::vector<int> dates;
	std::vector<float> dfs;
	for (int i = 1; i < nbRows; ++i)
	{
		date = Curves(i, offset * 2).AsInt();
		df = (float)Curves(i, offset * 2+1).AsDouble();
		dates.push_back(date);
		dfs.push_back(df);
	}
	yc->buildFromDFs(dates, dfs);
	return yc;
}

extern "C" LPXLFOPER __declspec(dllexport) xlMarketLoad(int marketDate, XlfOper Curves, XlfOper fxSpots, XlfOper fxVol, XlfOper irVol)
{
	EXCEL_BEGIN;

	// Checks if called from the function wizard
	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

//	holidays& holi = holidays::createHolidays();
	int next = spotDate(EUR, USD, 40084);
	next = spotDate(USD, TRY, 39856);
	next = spotDate(GBP, USD, 39984);
	next = spotDate(EUR, USD, 39932);

	next = spotDate(USD, CAD, 40025);
	next = spotDate(AUD, NZD, 40094);
	next = spotDate(USD, BRL, 40127);
	next = spotDate(USD, MXN, 40127);


	if (Curves.IsMissing())
	{
		return XlfOper("Curve parameter is missing");
	}
	marketData* md = new marketData(marketDate);
	int nbCurves = Curves.columns()/2;
	int i;
	for (i = 0; i < nbCurves; ++i)
	{
		Currency ccy = readycCurveCurrency(Curves, i);
		yieldCurve* yc = readycCurve(Curves, i, marketDate);
		if (yc)
		{
			md->addCurve(ccy, yc);
		}
	}

	if (! fxSpots.IsMissing())
	{
		// Set up FX table
		fxTable* fx = new fxTable;
		if (fxSpots.columns() != 2)
		{
			return XlfOper("FXSpot is required to have 2 columns.");
		}
		int nbRows = fxSpots.rows();
		std::string fxpair;
		float fxSpot;
		for (int i = 0; i < nbRows; ++i)
		{
			fxpair = fxSpots(i, 0).AsString();
			fxSpot = (float)fxSpots(i, 1).AsDouble();
			Currency fgn = toCurrency(fxpair.substr(0, 3).c_str());
			Currency dom = toCurrency(fxpair.substr(3).c_str());
			fx->setSpot(FXPair(fgn, dom), fxSpot);
		}
		if (fx)
		{
			md->addFXTable(fx);
		}
	}

	int h = handle.add(md);

	return XlfOper(h);
	EXCEL_END;
}


extern "C" LPXLFOPER __declspec(dllexport) xlMarketDelete(int h)
{
	EXCEL_BEGIN;

	// Checks if called from the function wizard
	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);


	marketData* md;
	if (handle.exists(h))
	{
		md = (marketData*)handle[h];
		delete md;
		return XlfOper(true);
	}
	else return XlfOper("Unknown market handle.");


	EXCEL_END;
}


extern "C" LPXLFOPER __declspec(dllexport) xlDF(int h, XlfOper CurrencyName, int date)
{
	EXCEL_BEGIN;

	// Checks if called from the function wizard
	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

	if (! handle.exists(h))
	{
		return XlfOper("Unknown market handle.");
	}

	marketData* md = (marketData*)handle[h];
	if (!md)
	{
		return XlfOper("Corrupted market handle.");
	}
	else
	{
		char* ccyString = CurrencyName.AsString();
		Currency ccy = toCurrency(ccyString);
		yieldCurve* yc = md->getCurve(ccy);
		if (!yc)
		{
			return XlfOper("Market does not have a curve for this currency.");
		}
		double df = yc->df(date);
		return XlfOper(df);
	}
	EXCEL_END;
} // extern "C" LPXLFOPER __declspec(dllexport) xlDF(int h, XlfOper CurrencyName, int date)


extern "C" LPXLFOPER __declspec(dllexport) xlFxSpot(int h, XlfOper FXPairString)
{
	EXCEL_BEGIN;

	// Checks if called from the function wizard
	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

	if (!handle.exists(h))
	{
		return XlfOper("Unknown market handle.");
	}


	marketData* md = (marketData*)handle[h];
	if (!md)
	{
		return XlfOper("Unknown market handle.");
	}
	else
	{
		std::string fgndom = FXPairString.AsString();

		FXPair fxpair = toFXPair(fgndom);

		fxTable* fx = md->getFxTable();
		if (!fx)
		{
			return XlfOper("Market does not have FX spot information.");
		}
		double fxspot = fx->getSpot(fxpair);
		return XlfOper(fxspot);
	}
	EXCEL_END;
} // xlDF(int h, XlfOper CurrencyName, int date)


extern "C" LPXLFOPER __declspec(dllexport) xlFxForward(int h, XlfOper FXPairString, int setDate)
{
	EXCEL_BEGIN;

	// Checks if called from the function wizard
	if (XlfExcel::Instance().IsCalledByFuncWiz())
		return XlfOper(true);

	if (!handle.exists(h))
	{
		return XlfOper("Unknown market handle.");
	}

	marketData* md = (marketData*)handle[h];
	if (!md)
	{
		return XlfOper("Corrupted market handle.");
	}
	else
	{
		std::string fgndom = FXPairString.AsString();

		FXPair fxp = toFXPair(fgndom);

		double FxForward = md->getFxForward(fxp, setDate);

		return XlfOper(FxForward);
	}
	EXCEL_END;
} // xlFxForward(int h, XlfOper FXPairString, int setDate)




namespace
{
	//MarketLoad
	XLRegistration::Arg MarketLoadArgs[] = {
		{ "marketDate", "Base date for the market", "J" },
		{ "Curves", "Yield Curves", "XLF_OPER" },
		{ "fxCurves", "FX table of {FGNDOM|level}", "XLF_OPER" },
		{ "fxVol", "fx volatility", "XLF_OPER" },
		{ "irVol", "ir volatility", "XLF_OPER" }
	};


	XLRegistration::XLFunctionRegistrationHelper registerMarketLoadArgs
		("xlMarketLoad", "MarketLoad", "Loads a market: DFs, fxSpots, fxVols and irVol.",
			"FinLib", MarketLoadArgs, 5);

	//MarketDelete
	XLRegistration::Arg MarketDeleteArgs[] = {
		{ "handle", "Handle for the market", "J" }
	};


	XLRegistration::XLFunctionRegistrationHelper registerMarketDeleteArgs
		("xlMarketLoad", "MarketDelete", "Delete a marked indentified by its handle.",
			"FinLib", MarketDeleteArgs, 1);

//DF
	XLRegistration::Arg DFArgs[] = {
		{ "marketHandle", "Handle to the market", "J" },
		{ "CurrencyName", "Currency Name", "XLF_OPER" },
		{ "date", "date", "J" }
	 };


	XLRegistration::XLFunctionRegistrationHelper registerDFArgs
		("xlDF", "DF", "Discount factor.",
			"FinLib", DFArgs, 3);

//FXSpot
	XLRegistration::Arg FxSpotArgs[] = {
		{ "marketHandle", "Handle to the market", "J" },
		{ "fxpair", "fxpair", "XLF_OPER" } 
	};


	XLRegistration::XLFunctionRegistrationHelper registerFxSpotArgs
		("xlFxSpot", "fxSpot", "FX spot rate.",
			"FinLib", DFArgs, 2);


//FXForward
	XLRegistration::Arg FxForwardArgs[] = {
		{ "marketHandle", "Handle to the market", "J" },
		{ "fxpair", "fxpair", "XLF_OPER" }, 
		{ "setDate", "Set Date", "J" }
	};


	XLRegistration::XLFunctionRegistrationHelper registerFxForwardArgs
		("xlFxForward", "fxForward", "Implied FX Forward rate.",
			"FinLib", DFArgs, 3);

//OptionPricer
	XLRegistration::Arg OptionPricerArgs[] = {
        { "Forward", "Forward", "B" },
        { "Strike", "Strike of the Option", "B" },
        { "AtmVol", "ATM Volatility", "B" },
        { "Time", "Time to Expiry", "B" },
        { "Model", "Model Descriptor", "XLF_OPER" }
    };

    XLRegistration::XLFunctionRegistrationHelper registerOptionPricer
	(   "xlOptionPricer", "OptionPricer", "Computes a call option price",
        "FinLib", OptionPricerArgs, 5);


//BlackScholes
	XLRegistration::Arg BlackScholesArgs[] = {
        { "Forward", "Forward", "B" },
        { "Strike", "Strike of the Option", "B" },
        { "AtmVol", "ATM Volatility", "B" },
        { "Time", "Time to Expiry", "B" }
    };

    XLRegistration::XLFunctionRegistrationHelper registerBlackScholes
	(   "xlBlackScholes", "BlackScholes", "BS formula for Call option",
        "FinLib", BlackScholesArgs, 4);
}
