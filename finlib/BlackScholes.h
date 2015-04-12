#ifndef _BLACKSCHOLES_H_
#define _BLACKSCHOLES_H_

typedef enum {CALL=0, PUT, DownDigital} OptionType;

double black(double F, double K, double sigma, double t, OptionType ot = CALL);
double normalblack(double F, double K, double sigma, double t, OptionType ot = CALL);
double hagan(double F0, double K, double sigma0, double volvol, double rho, double beta, double t, OptionType ot = CALL);


//Lognormal Black formula for a European call
class lognormalBS
{
public:
	lognormalBS(double _F, double _K, double _sigma, double _t);
	double delta();
	double gamma();
	double vega();
	double theta();
	double value();
private:
	double F;
	double K;
	double sigma;
	double t;
// temp storage 
	double d1;
	double sigmasqrt;
};

#endif