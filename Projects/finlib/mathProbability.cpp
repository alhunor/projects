#include <gsl\gsl_sf.h>
#include "mathProbability.h"
#define _USE_MATH_DEFINES
#include <math.h>


double gaussPdf(double x)
{
	return gsl_sf_erf_Z(x);
};

double gaussPdf(double x, double mu, double sigma)
{
	double aux=1/sigma;
	return gaussPdf((x-mu)*aux)*aux;
};

double gaussCdf(double x)
{
//	return 0.5*(1+gsl_sf_erf(x)*M_SQRT1_2);
	return 1 - gsl_sf_erf_Q(x);
};

double gaussCdf(double x, double mu, double sigma)
{
	return gaussCdf((x-mu)/sigma);
};

double normalInverse(double d) {return d;}