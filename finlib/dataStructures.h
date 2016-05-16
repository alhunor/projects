#ifndef DataStructures_H_
#define DataStructures_H_

//#include <boost/shared_ptr.hpp>
#include <map>

//#include <windows.h>

typedef enum {
	AED = 0, AFN, ALL, AMD, ANG, AOA, ARS, AUD, AWG, AZN,
	BAM, BBD, BDT, BGN, BHD, BIF, BMD, BND, BOB, BRL, BSD, BTN, BWP, BZD, CAD, CDF, CHF, CLP, CNY,
	COP, CRC, CUP, CVE, CZK, DJF, DKK, DOP, DZD, EGP, ERN, ETB, EUR, FJD, FKP, GBP, GEL, GHS, GIP,
	GMD, GNF, GTQ, GYD, HKD, HNL, HRK, HTG, HUF, IDR, ILS, INR, IQD, ISK, JMD, JOD, JPY, KES,
	KGS, KHR, KMF, KPW, KRW, KWD, KYD, KZT, LAK, LBP, LKR, LRD, LSL, LYD, MAD, MDL, MGA, MKD, MMK,
	MNT, MOP, MRO, MUR, MVR, MWK, MXN, MYR, MZN, NAD, NGN, NIO, NOK, NPR, NZD, OMR, PAB, PEN, PGK,
	PHP, PKR, PLN, PYG, QAR, RON, RSD, RUB, RWF, SAR, SBD, SCR, SDG, SEK, SGD, SHP, SLL, SOS, SRD,
	SYP, SZL, THB, TJS, TMT, TND, TOP, TRY, TTD, TWD, TZS, UAH, UGX, USD, UYU, UZS, VEF, VUV, WST,
	XAF, XCD, XOF, XPF, YER, ZAR, ZMW, UNKNOWN
}	Currency;


struct FXPair
{
	FXPair() {}
	FXPair(Currency _fgn, Currency _dom) : fgn(_fgn), dom(_dom) {}
	FXPair& operator=(const FXPair& fxp) { fgn = fxp.fgn; dom = fxp.dom; return *this; }
	Currency fgn, dom;
};


typedef struct 
{
	time_t time;
	float bid, ask;
} timeStampedBidAskT;


/////////////   Handles      ///////////////////////////////////

class handleT
{
public:
	handleT() {n=0;}

	int get() {handles[n]=NULL; return n++;}
	int add(void* p);
	void*& operator[](int i);
	void remove(int h);
	int nbHandles() const {return n;}
	bool exists(int h);
	void clear();

private:
    handleT(handleT const&);         // Don't Implement
    void operator=(handleT const&); // Don't implement
	std::map<int, void*> handles;
	std::map<int, void*>::iterator it;
	int n;
};



struct valueT
{
	bool isString;
	void* p;
};

class labelValue
{
public:
	labelValue() { }
	~labelValue() { clear(); }
	void add(std::string name, std::string value);// {	data[name]=new std::string(value);}
	void add(std::string name, double value);
	std::string& getStr(std::string name);
	double getNum(std::string name);
	bool setNum(std::string name, double value);
	void* getObj(std::string name);
	void* getPointer(std::string name);
	int getHandle(std::string name); // return -1 if label is not present
	Currency getCurrency(std::string name);
	FXPair getFXPair(std::string name);

	void clear();

	std::map<std::string, valueT> data;
};


#endif