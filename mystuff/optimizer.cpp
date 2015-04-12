#include "optimizer.h"



//#include <gsl/gsl_math.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_integration.h>
#include <gsl/gsl_multiroots.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_poly.h>
#include <gsl/gsl_roots.h>
#include <gsl/gsl_vector.h>

double gsl_rootFinder(gslfuncptr f, double lo, double hi, void* params, double tol, int maxiter)
{
	double flo =f(lo, params);
	double fhi =f(hi, params);

	if (flo*fhi>=0)
		throw "No root between lo..hi";

	gsl_function func;
	func.function=f;
	func.params=params;

	const gsl_root_fsolver_type *T = gsl_root_fsolver_brent;
	gsl_root_fsolver* solver = gsl_root_fsolver_alloc(T);
	gsl_root_fsolver_set(solver, &func, lo, hi);

	int i=0;
	double x=0; //root
	int status;

	do
	{
		i++;
		status = gsl_root_fsolver_iterate(solver);
		x = gsl_root_fsolver_root(solver);
		lo = gsl_root_fsolver_x_lower(solver);
		hi = gsl_root_fsolver_x_upper(solver);
		status = gsl_root_test_interval(lo, hi, tol, 0);
	
	} while (status== GSL_CONTINUE && i<maxiter);
	gsl_root_fsolver_free(solver);
	return x;
} // double gsl_rootFinder(gslfuncptr f, double lo, double hi, void* params, double tol, int maxiter)


gsl_vector* gsl_multirootFinder(gslmultifuncptr f, int n, double* x0, void* params, double tol, int maxiter)
{
	int i;
	gsl_vector* roots = gsl_vector_calloc(n);
	
	// store initial guess x0
	gsl_vector* x = gsl_vector_calloc(n);
	for (i=0; i<n; ++i)
	{
		gsl_vector_set(x,i, x0[i]);
	}

	gsl_multiroot_function func;
	func.f=f;
	func.params=params;
	func.n=n;

	const gsl_multiroot_fsolver_type *T = gsl_multiroot_fsolver_dnewton;
	gsl_multiroot_fsolver* solver = gsl_multiroot_fsolver_alloc(T, n);
	gsl_multiroot_fsolver_set(solver, &func, x);

	i=0;
	int status;

	do
	{
		i++;
		roots = gsl_multiroot_fsolver_root(solver);

		status = gsl_multiroot_fsolver_iterate(solver);

		status = gsl_multiroot_test_residual(solver->f, tol);
	
	} while (status== GSL_CONTINUE && i<maxiter);
	gsl_multiroot_fsolver_free(solver);
	gsl_vector_free(x);

	return roots;
} // double gsl_rootFinder(gslfuncptr f, double lo, double hi, void* params, double tol, int maxiter)


HuMatrix<double> gsl_polyRoots(int n, double* a)
{
	// P(x) = a0+a1*x+a2*x^2+.. an-1*x(n-1)
	// a[0]=+1; a[1]=0; a[2]=0; a[3]=1; => X^3-1

	gsl_poly_complex_workspace *w = gsl_poly_complex_workspace_alloc(n);
	gsl_set_error_handler_off();
	double* z = new double[2*(n-1)];

	int status= gsl_poly_complex_solve(a,n,w,z);
	// Roots are returned in Z alternating real and imaginary parts
	if (status!=0)
	{
		if (status== GSL_EFAILED)
			throw "QR did not converge.";
		if (status== GSL_EINVAL)
			throw "Invalid coefficients.";
		throw "gsl_polyRoots has failed";
	}
	gsl_poly_complex_workspace_free(w);

	HuMatrix<double> roots(n-1,2);
	for (int i=0; i<(n-1); i++)
	{
		roots(i,0)=z[2*i]; //Real
		roots(i,1)=z[2*i+1];//imaginary
	}
	delete[] z;
	return roots;
} // HuMatrix<double> gsl_polyRoots(int n, double* coeffs)


// Adaptive integration (QAG) with 21 points Gauss-Konrod method
double gsl_polyIntegrator(double* fgrid, double *xgrid, int nPoints, int polyorder)
{
	gsl_integration_workspace * w =gsl_integration_workspace_alloc(500);
	//sigma, coeffs
	//coeffs = polyFit1D(xgrid, fgrid, sigma, polyorder);
	//	gsl_integration_qagp(F, )

	gsl_integration_workspace_free(w);
	return 0;
} //double gsl_polyIntegrator(double* fgrid, double *xgrid, int nPoints, int olyorder)


gsl_vector* gsl_multiLinearRegression(const gsl_matrix* X, const gsl_vector* f)
{
	// n observations with p parameters
	// Solves the problem f = X*c and returns c.
	int n, p;
	n= X->size1;
	p= X->size2;
	double chi2;

	if (f->size!=n)  throw "gsl_multiLinearRegression::Mismatched dimensions.";

	gsl_multifit_linear_workspace* w = gsl_multifit_linear_alloc(n,p);

	gsl_vector* c = gsl_vector_calloc(p);
	gsl_matrix* cov = gsl_matrix_calloc(p, p);

	gsl_multifit_linear(X, f, c, cov, &chi2, w);

	gsl_multifit_linear_free(w);
	gsl_matrix_free(cov);

	return c;
} // gsl_vector* gsl_multiLinearRegression(gsl_matrix* X, gsl_vector* f)




gsl_vector* gsl_polyfit(const gsl_vector* x, const gsl_vector* f, int polyOrder)
{
	int n = x->size;
	if (n!=f->size) throw "gsl_polyfit::Mismatched dimensions.";
	gsl_matrix* basis = gsl_matrix_alloc(n, polyOrder);
	int i,j;
	double tmp;
	double* xPtr = x->data;
	for (i=0; i<n; i++)
	{
		gsl_matrix_set(basis, i, 0, 1);
		tmp = 1;
		for (j=1; j<polyOrder; ++j)
		{
			tmp*=xPtr[i];
			gsl_matrix_set(basis, i, j, tmp);
		}
	}
	gsl_vector* c = gsl_multiLinearRegression(basis, f);
	gsl_matrix_free(basis);
	return c;
} // gdl_vector* gsl_polyfit(gsl_vector x, gsl_vector* f, int n, int p)