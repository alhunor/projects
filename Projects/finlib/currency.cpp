#include <algorithm>
#include <fstream>
#include <map>
#include <vector>

#include "mystuff/binsearch.h"
#include "currency.h"




struct { Currency ccy; char* s; } currencies[150] = {
	{ USD,"USD" },
	{ CHF,"CHF" },
	{ CAD,"CAD" },
	{ GBP,"GBP" },
	{ EUR,"EUR" },
	{ AUD,"AUD" },
	{ JPY,"JPY" },
	{ AED,"AED" },
	{ AFN,"AFN" },
	{ ALL,"ALL" },
	{ AMD,"AMD" },
	{ ANG,"ANG" },
	{ AOA,"AOA" },
	{ ARS,"ARS" },
	{ AWG,"AWG" },
	{ AZN,"AZN" },
	{ BAM,"BAM" },
	{ BBD,"BBD" },
	{ BDT,"BDT" },
	{ BGN,"BGN" },
	{ BHD,"BHD" },
	{ BIF,"BIF" },
	{ BMD,"BMD" },
	{ BND,"BND" },
	{ BOB,"BOB" },
	{ BRL,"BRL" },
	{ BSD,"BSD" },
	{ BTN,"BTN" },
	{ BWP,"BWP" },
	{ BZD,"BZD" },
	{ CDF,"CDF" },
	{ CLP,"CLP" },
	{ CNY,"CNY" },
	{ COP,"COP" },
	{ CRC,"CRC" },
	{ CUP,"CUP" },
	{ CVE,"CVE" },
	{ CZK,"CZK" },
	{ DJF,"DJF" },
	{ DKK,"DKK" },
	{ DOP,"DOP" },
	{ DZD,"DZD" },
	{ EGP,"EGP" },
	{ ERN,"ERN" },
	{ ETB,"ETB" },
	{ FJD,"FJD" },
	{ FKP,"FKP" },
	{ GEL,"GEL" },
	{ GHS,"GHS" },
	{ GIP,"GIP" },
	{ GMD,"GMD" },
	{ GNF,"GNF" },
	{ GTQ,"GTQ" },
	{ GYD,"GYD" },
	{ HKD,"HKD" },
	{ HNL,"HNL" },
	{ HRK,"HRK" },
	{ HTG,"HTG" },
	{ HUF,"HUF" },
	{ IDR,"IDR" },
	{ ILS,"ILS" },
	{ INR,"INR" },
	{ IQD,"IQD" },
	{ ISK,"ISK" },
	{ JMD,"JMD" },
	{ JOD,"JOD" },
	{ KES,"KES" },
	{ KGS,"KGS" },
	{ KHR,"KHR" },
	{ KMF,"KMF" },
	{ KPW,"KPW" },
	{ KRW,"KRW" },
	{ KWD,"KWD" },
	{ KYD,"KYD" },
	{ KZT,"KZT" },
	{ LAK,"LAK" },
	{ LBP,"LBP" },
	{ LKR,"LKR" },
	{ LRD,"LRD" },
	{ LSL,"LSL" },
	{ LYD,"LYD" },
	{ MAD,"MAD" },
	{ MDL,"MDL" },
	{ MGA,"MGA" },
	{ MKD,"MKD" },
	{ MMK,"MMK" },
	{ MNT,"MNT" },
	{ MOP,"MOP" },
	{ MRO,"MRO" },
	{ MUR,"MUR" },
	{ MVR,"MVR" },
	{ MWK,"MWK" },
	{ MXN,"MXN" },
	{ MYR,"MYR" },
	{ MZN,"MZN" },
	{ NAD,"NAD" },
	{ NGN,"NGN" },
	{ NIO,"NIO" },
	{ NOK,"NOK" },
	{ NPR,"NPR" },
	{ NZD,"NZD" },
	{ OMR,"OMR" },
	{ PAB,"PAB" },
	{ PEN,"PEN" },
	{ PGK,"PGK" },
	{ PHP,"PHP" },
	{ PKR,"PKR" },
	{ PLN,"PLN" },
	{ PYG,"PYG" },
	{ QAR,"QAR" },
	{ RON,"RON" },
	{ RSD,"RSD" },
	{ RUB,"RUB" },
	{ RWF,"RWF" },
	{ SAR,"SAR" },
	{ SBD,"SBD" },
	{ SCR,"SCR" },
	{ SDG,"SDG" },
	{ SEK,"SEK" },
	{ SGD,"SGD" },
	{ SHP,"SHP" },
	{ SLL,"SLL" },
	{ SOS,"SOS" },
	{ SRD,"SRD" },
	{ SYP,"SYP" },
	{ SZL,"SZL" },
	{ THB,"THB" },
	{ TJS,"TJS" },
	{ TMT,"TMT" },
	{ TND,"TND" },
	{ TOP,"TOP" },
	{ TRY,"TRY" },
	{ TTD,"TTD" },
	{ TWD,"TWD" },
	{ TZS,"TZS" },
	{ UAH,"UAH" },
	{ UGX,"UGX" },
	{ UYU,"UYU" },
	{ UZS,"UZS" },
	{ VEF,"VEF" },
	{ VUV,"VUV" },
	{ WST,"WST" },
	{ XAF,"XAF" },
	{ XCD,"XCD" },
	{ XOF,"XOF" },
	{ XPF,"XPF" },
	{ YER,"YER" },
	{ ZAR,"ZAR" },
	{ ZMW,"ZMW" } };


bool operator<(const FXPair fxp1, const FXPair fxp2)
{
	return fxp1.key < fxp2.key;
}

Currency toCurrency(const char* str)
{
	for (int i = 0; i < 150; ++i)
	{
		if (strcmp(str, currencies[i].s) == 0)
		{
			return currencies[i].ccy;
		}
	}
	throw "Unknown currency: " + std::string(str);
} // Currency toCurrency(char* str)


FXPair toFXPair(std::string& fgndom)
{
	Currency dom, fgn;
	fgn = toCurrency(fgndom.substr(0, 3).c_str());
	dom = toCurrency(fgndom.substr(3).c_str());
	return FXPair(fgn,dom);
} // FXPair toFXPair(std::string& s)



bool operator< (holiday a, holiday b)
{
	return a.date < b.date;
}

bool operator> (holiday a, holiday b)
{
	return a.date > b.date;
}


bool holidays::init(char* filename)
{
	dates.clear();
	std::fstream in;
	in.open(filename);
	if (in.fail())
	{
		throw "Unable to open holiday file.";
	}
	while (!in.eof()) // !in.is_open()
	{
		int nbHolidays;
		char currencyString[4];
		in >> currencyString >> nbHolidays;
		Currency ccy = toCurrency(currencyString);
		dates[ccy].resize(nbHolidays);
		for (int i = 0; i < nbHolidays; ++i)
		{
			char description[64];
			int date;
			in >> date;
			in.getline(description, 63);
			dates[ccy][i].date = date;
			dates[ccy][i].description = description;
		}
	}
	return true;
}

bool holidays::isHoliday(Currency ccy, int date)
{
	holiday h;
	h.date = date;
	if (dates.find(ccy) == dates.end()) return false;
	//
	int bin = binsearch(h, &(dates[ccy][0]), 0, dates[ccy].size());
	return bin >= 0;
}



bool isBizDay(int date, Currency ccy)
{
	// checks for weekends
	if (date % 7 < 2) return false; // Exceldate modulo 7 gives the day of the week, starting as 0 - Sat, 1- Sun, .. 6 - Friday

									// then check for public holidays for currency
	holidays& holi = holidays::createHolidays();
	return !holi.isHoliday(ccy, date);
}


int nextBizDay(int date, Currency ccy)
{
	while (!isBizDay(date, ccy)) ++date;
	return date;
}

bool specialCurrency(Currency ccy)
{
	return ccy == MXN || ccy == ARS || ccy == CLP;
}

int settlementDelay(Currency ccy1, Currency ccy2)
{
	switch (ccy1)
	{
	case USD:
		switch (ccy2)
		{
		case CAD:
			return 1;
		case TRY:
			return 1;
		case RUB:
			return 1;
		default:
			return 2;
		}
	case EUR:
	case CAD:
		switch (ccy2)
		{
		case TRY:
			return 1;
		case RUB:
			return 1;
		default:
			return 2;
		}
		break;
	case TRY:
		if (ccy2 == RUB) return 1; else return 2;
	default:
		return 2;
	}
	return 2;
} // int settlementDelay(Currency ccy1, Currency ccy2)


int spotDate(Currency ccy1, Currency ccy2, int date)
{
	int k = settlementDelay(ccy1, ccy2);
	if (k == 2)
	{
		if (specialCurrency(ccy1) || specialCurrency(ccy2))
		{
			// advance forward 1 good business day, skippping holidays in ccy1, ccy2, USD
			do
			{
				++date;
			} while (!isBizDay(date, ccy1) || !isBizDay(date, ccy2) || !isBizDay(date, USD));
		}
		else // advance forward 1 good business day, skippping holidays in ccy1, ccy2 unless ccy1 is USD or ccy2 is USD
		{
			do
			{
				++date;
				if (ccy1 == USD || ccy2 == USD) break;
			} while (! isBizDay(date, ccy1) || ! isBizDay(date, ccy2));
		}
	}
	if (k == 2 || k == 1 || (k == 0 && (!isBizDay(date, ccy1) || !isBizDay(date, ccy2) || !isBizDay(date, USD)) ))
	{
		// advance forward 1 good business day, skippping holidays in ccy1, ccy2, USD
		do
		{
			++date;
		} while (! isBizDay(date, ccy1) || ! isBizDay(date, ccy2) || !isBizDay(date, USD));
	}
	return date;
} // int spotDate(Currency ccy1, Currency ccy2, int date)