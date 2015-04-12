#include <math.h>
#include "BlackScholes.h"
#include "mathProbability.h"
// formulae from http://en.wikipedia.org/wiki/Black%E2%80%93Scholes with r=0 and for a call.

inline static double sq(double x) {return x*x;}


double black(double F, double K, double sigma, double t, OptionType ot)
{
	// prices a call option
	double sigmasqrt = sigma*sqrt(t);
	double d1 = (log(F/K)+0.5*sigma*sigma*t)/sigmasqrt;

	if (ot == CALL)
	{
		return F*gaussCdf(d1)-K*gaussCdf(d1-sigmasqrt);
	} else
	{
		//return F*gaussCdf(d1)-K*gaussCdf(d1-sigmasqrt);
		throw "Not implemented";
	}
};


double normalblack(double F, double K, double sigma, double t, OptionType ot)
{
	// prices a call option
	double sigmasqrt = sigma*sqrt(t);
	double d =(F-K)/sigmasqrt;
	
	if (ot == CALL)
	{
		return (F-K)*gaussCdf(d)-sigmasqrt * exp(-d*d*0.5)*M_1_SQRT2PI;
	} else
	{
		return (K-F)*gaussCdf(-d)-sigmasqrt * exp(-d*d*0.5)*M_1_SQRT2PI;
	}
};


double hagan(double F0, double K, double sigma0, double volvol, double rho, double beta, double t, OptionType ot)
{
	double gamma1 = beta/F0;
	double gamma2 = beta*(beta-1)/(F0*F0);
	double aux =  pow(F0*K, 0.5*(1-beta));
	double logf_k = log(F0/K);
	double z = volvol/sigma0 * aux * logf_k;
	double xz = log((sqrt(1-2*rho*z+z*z)+z-rho)/(1-rho));

	double term = 1+ t * ( sq(sigma0*(1-beta)/aux)/24+ 0.25*volvol*sigma0*beta*rho/aux + (2-3*rho*rho)/24*volvol) ;
	double sigma = volvol * log(F0/K)/(1+ sq((1-beta)*logf_k)/24+sq(sq((1-beta)*logf_k))/1920)/xz * term;

	// special case of sigmaAtm when F0=K as per eq 2.18
	
	return black(F0,K, sigma,t, ot);
};


double normalhagan(double F0, double K, double sigma0, double volvol, double rho, double beta, double t, OptionType ot = CALL)
{
	double sigma = 0;
	// special case of sigmaAtm when F0=K as per eq 2.18
	
	return normalblack(F0,K, sigma,t, ot);
};

lognormalBS::lognormalBS(double _F, double _K, double _sigma, double _t): F(_F), K(_K), sigma(_sigma), t(_t)
{
	sigmasqrt = sigma*sqrt(t);
	d1=(log(F/K)+0.5*sigma*sigma*t)/sigma*sqrt(t);
}


double lognormalBS::value()
{
	return F*gaussCdf(d1)-K*gaussCdf(d1-sigma*sqrt(t));
}

double lognormalBS::delta()
{
	return gaussCdf(d1);
}


double lognormalBS::gamma()
{
	return gaussPdf(d1)/(F*sqrt(t)*sigma);
}


double lognormalBS::vega()
{
	return F*sqrt(t)*gaussPdf(d1);
}

double lognormalBS::theta()
{
	return -F*gaussPdf(d1)*sigma/(2*sqrt(t));	
}

