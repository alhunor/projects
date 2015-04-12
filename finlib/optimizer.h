#ifndef _OPIMIZER_H_
#define _OPIMIZER_H_
#include <gsl\gsl_vector.h>
#include <gsl\gsl_matrix.h>
#include "matrix.h"

typedef double (*gslfuncptr)(double, void*);
typedef int (*gslmultifuncptr)(const gsl_vector* , void*, gsl_vector* );

double gsl_rootFinder(gslfuncptr f, double lo, double hi, void* params, double tol=1e-6, int maxiter = 100);
gsl_vector* gsl_multirootFinder(gslmultifuncptr f, int n, double* x0, void* params, double tol=1e-6, int maxiter = 500);

HuMatrix<double> gsl_polyRoots(int n, double* coeffs);

double gsl_polyIntegrator(double* fgrid, double *xgrid, int nPoints, int polyOrder); // not finished

gsl_vector* gsl_multiLinearRegression(const gsl_matrix* X, const gsl_vector* f);

gsl_vector* gsl_polyfit(const gsl_vector* x, const gsl_vector* f, int polyOrder);

#endif