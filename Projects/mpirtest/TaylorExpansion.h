#ifndef TAYLOR_EXPANSION_H_
#define TAYLOR_EXPANSION_H_

#include "mpir.h"

class TaylorExpansion
{
public:
	TaylorExpansion();
	~TaylorExpansion();
	virtual bool initCoeffs(unsigned int order,  unsigned int _precision) = 0;
	bool evaluate(mpf_t& res, double x);
	bool evaluate(mpf_t& res, mpf_t x);
protected:
	mpf_t* coeffs;
	unsigned int precision;
	unsigned int nb; // order of the expansion
	enum {Even, Odd, All} summationType;

	// temporary variables, initilaised once to save time
	mpf_t x2, xk, term;
	static mpf_t flt_min, dbl_min;
	static bool initialised;
};



class cosine: public TaylorExpansion
{
public:
	cosine() {summationType = Even;}
	virtual bool initCoeffs(unsigned int order,  unsigned int precision);
};

class sine: public TaylorExpansion
{
public:
	sine() {summationType = Odd;}
	virtual bool initCoeffs(unsigned int order,  unsigned int precision);
};

class inverf: public TaylorExpansion
{
public:
	inverf() {summationType = Odd;}
	virtual bool initCoeffs(unsigned int order,  unsigned int precision);
// see http://mathworld.wolfram.com/InverseErf.html
};


class exponential: public TaylorExpansion
{
public:
	exponential() {summationType = All;}
	virtual bool initCoeffs(unsigned int order,  unsigned int precision);
};

void computeErfCoefficients();

#endif