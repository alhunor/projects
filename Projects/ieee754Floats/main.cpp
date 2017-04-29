#include <iostream>
#include "conio.h"

union Float_t
{
	Float_t(float val = 0.0f) : f(val) {}
	bool negative() const { return (i >> 31) != 0; }
	bool mantissa() const { return i && ((i << 23) - 1); }
	bool exponent() const { return i && (i >> 23) & 0xFF; }
	int32_t i;
	float f;
	struct
	{
		uint32_t mantissa : 23;
		uint32_t rawExponent : 8; // Substarct 127 to get current value
		uint32_t sign : 1; // 0 for positive, 1 for negative
	} parts;
	// if 0<rawExponent<255 => value = (-1)^sign * (1+ mantissa/2^24)*2^(exponent-127), 
	// if rawExponent = 0 => value = (-1)^sign * mantissa/2^149
	// if rawExponent = 255 => value = inf, NaN, see standard for exact meaning
};


char* itoBinary(unsigned int i, char* buff, char bufflen)
{
	buff[bufflen] = 0;
	bufflen--;
	while (i > 0)
	{
		buff[bufflen] = 48 + i % 2;
		i >>= 1;
		--bufflen;
		if (bufflen < 0) throw "buff is too short";
	}
	return &buff[bufflen+1];
}

using namespace std;

float invCDF(float y) { return sqrt(1.0f / (1.0f - y)); }

void main()
{
	double d = 0;
	double sumf = 0; // sum of all floats
	float f; int& i = (int&)f;

	// scanning all positive denormalised floats
	f = 0.0f; // skipping zero
	int nbfloats_den = 0;
	for (; f < FLT_MIN; ++i) // strict inaquality is right here
	{
		++nbfloats_den;
		//d += invCDF(f);
		sumf += f;
	}
	cout << "There are " << nbfloats_den << " denormalised floats between [0 .. FLT_MIN)." << endl; // should be 8 388 608

	d = 0;
	nbfloats_den = 0;
	sumf = 0;
	// scanning all normalised floats in [FLT_MIN 1) intervall
	f = FLT_MIN;
	float tmp, tmp2;
	int nbfloats = 0;
	double partialSum;
	int mantissa;
	for (; f < 1; )
	{
		partialSum = 0;
		tmp = f;
		for (mantissa=0; mantissa < 8388608; ++mantissa)
		{
			++nbfloats;
			//partialSum += f;
			partialSum += invCDF(f);
			++i;
		}
		// (f-tmp) is the difference between two consecutive floats when the exponent is increased. 
		// the length of the previous interval is 2 * (f-tmp)
		sumf += partialSum * 2 * (f-tmp);
	}
	cout << "There are " << nbfloats << " floats in the [FLT_MIN,1) open inerval." << endl; // should be 1 056 964 608
	d = d / (nbfloats_den + nbfloats);

	cout << "Average all floats between [0,1) is " << 0.5 * sumf / 8388608 <<endl; // should be 1 056 964 608
	d = d / (nbfloats_den + nbfloats);



	const char bufflen = 4;
	char buff[4];
	Float_t n(1.0f);
	n.i -= 1;

	try {
		cout << itoBinary(1, buff, bufflen) << endl;
		cout << itoBinary(2, buff, bufflen) << endl;
		cout << itoBinary(4, buff, bufflen) << endl;
		cout << itoBinary(8, buff, bufflen) << endl;
		cout << itoBinary(16, buff, bufflen) << endl;
		cout << itoBinary(32, buff, bufflen) << endl;
		cout << itoBinary(48, buff, bufflen) << endl;
		cout << itoBinary(3, buff, bufflen) << endl;
	}
	catch (...)
	{
		cout << "Tada error" << endl;
	}
	for (;;)
	{
		printf("%1.8e, 0x%08X, %d, %d, 0x%06X\n", n.f, n.i, n.parts.sign, n.parts.rawExponent, n.parts.mantissa);
	}

	cout << "Terminated. Press any key to exit...";
	_getch();
}