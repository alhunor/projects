#pragma once

#include <map>
#include <string>
#include <vector>

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
	FXPair() = delete;
	FXPair(Currency _fgn, Currency _dom) : fgn(_fgn), dom(_dom), key(UNKNOWN*fgn+dom) {}
	FXPair& operator=(const FXPair& fxp) { fgn = fxp.fgn; dom = fxp.dom; key = fxp.key;  return *this; }
	Currency fgn, dom;
	int key;
};

bool operator<(const FXPair fxp1, const FXPair fxp2);

struct holiday { std::string description; int date; };
//bool operator< (holiday a, holiday b);
//bool operator> (holiday a, holiday b);

class holidays
{
private:
	holidays() { init("Currency holidays.csv"); };
	holidays(holidays const&);
	void operator=(holidays const&);
public:
	static holidays& createHolidays()
	{
		static holidays h;
		return h;
	};
	static bool init(char* fileName);
	static bool isHoliday(Currency ccy, int date);
protected:
	static std::map <Currency, std::vector<holiday> > dates;
};


Currency toCurrency(const char* str);
FXPair toFXPair(std::string& fgndom);


bool isBizDay(int date, Currency ccy);
int nextBizDay(int date, Currency ccy);
int spotDate(Currency ccy1, Currency ccy2, int date);