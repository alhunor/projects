#pragma once
#include <map>
#include <vector>

#include "dataStructures.h"
#include "marketData.h"
#include "myStuff/Matrix.h"
#include "blackScholes.h"


class trade
{
public:
	trade() = delete;
	trade(labelValue& lv);
	virtual double price() = 0;
	virtual double delta() = 0;
	virtual bool check() { return initialised; }; // checks if object is in okay state, throws error

protected:
	bool initialised;
	Currency notionalCCy;
	double notional;
};


struct time
{
	int t;
	double domDF, fgnDF; // between today and <t>
	double fxForward; // exchanged right in <t>
	bool init(FXPair& fxp, int time, marketData* md, double fxSpot, double fgnRate, double domRate);
};

struct calendarline
{
	calendarline() {}
	bool init(FXPair& fxp, labelValue& lv, marketData* md);
	time spot, expiry, delivery;
};

class fxForwardTrade : public trade
{
public:
	fxForwardTrade() = delete;
	fxForwardTrade(labelValue& lv, marketData* md);
	bool check(); // checks if object is in okay state, throws error
	double price() { return (cal.expiry.fxForward - strike)*notional*cal.delivery.domDF; } // delivery=expiry for outright FX forward, but different for NDF
	double delta() { return notional*cal.delivery.domDF; } // TODO XXX wrong formula
protected:
	double strike;
	FXPair fxp;
	calendarline cal;
};


class fxVanillaOption : public trade // Prices European Call, Put, UpDigital, DownDigital
{
public:
	fxVanillaOption() = delete;
	fxVanillaOption(labelValue& lv, marketData* md);
	bool check(); // checks if object is in okay state, throws error
	double price();
	double delta(); // TODO XXX wrong formula
protected:
	FXPair fxp;
	double strike;
	double vol;
	calendarline cal;
	FXOptionType tipus; // declared in blackScholes.h
	marketData* md;
};


VariantArray valueTrade(labelValue& lv, marketData* md, std::string& cs, trade* trade=NULL);
