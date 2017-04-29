#pragma once

typedef enum {Call = 0, Put, UpDigital, DownDigitial } OptionType;
typedef enum {SimpleDelta = 0, adjustedXX} DeltaConvention;

class fxoption
{
public:
	fxoption();
	~fxoption();
	double price(OptionType ot, float t, float K);
	double delta(DeltaConvention dc, float t, float K);
protected:

};
