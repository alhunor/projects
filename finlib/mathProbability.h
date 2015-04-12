#ifndef _MATHPROBABILITY_H_
#define _MATHPROBABILITY_H_

#define _USE_MATH_DEFINES
#include <math.h>
const double M_1_SQRT2PI = 0.398942280401433;
/* _USE_MATH_DEFINES are as below
M_E			e			2.718281828
M_LOG2E		log2(e)		1.442695041
M_LOG10E	log10(e)	0.434294482
M_LN2		ln(2)		0.693147181
M_LN10		ln(10)		2.302585093
M_PI		pi			3.141592654
M_PI_2		pi/2		1.570796327
M_PI_4		pi/4		0.785398163
M_1_PI		1/pi		0.318309886
M_2_PI		2/pi		0.636619772
M_2_SQRTPI	2/sqrt(pi)	1.128379167
M_SQRT2		sqrt(2)		1.414213562
M_SQRT1_2	1/sqrt(2)	0.707106781
*/


// returns gaussian pdf(x)=1/sqrt(2PI)*exp(-x^2/2)
double gaussPdf(double x);

double gaussPdf(double x, double mu, double sigma);

double gaussCdf(double x);

double gaussCdf(double x, double mu, double sigma);

double normalInverse(double d);

#endif