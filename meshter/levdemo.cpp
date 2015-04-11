#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include <levmar.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Gaussian noise with mean m and variance s, uses the Box-Muller transformation
double gNoise(double m, double s)
{
	double r1, r2, val;
	r1=((double)rand())/RAND_MAX;
	r2=((double)rand())/RAND_MAX;

	val=sqrt(-2.0*log(r1))*cos(2.0*M_PI*r2);
	val=s*val+m;
	return val;
}

// model to be fitted to measurements: x_i = p[0]*exp(-p[1]*i) + p[2], i=0...n-1
void expfunc(double *p, double *x, int m, int n, void *data)
{
	register int i;
	for(i=0; i<n; ++i)
	{
		x[i]=p[0]*exp(-p[1]*i) + p[2];
	}
}

// Jacobian of expfunc()
void jacexpfunc(double *p, double *jac, int m, int n, void *data)
{   
	register int i, j;
  
  // fill Jacobian row by row
	for(i=j=0; i<n; ++i)
	{
		jac[j++]=exp(-p[1]*i);
		jac[j++]=-p[0]*i*exp(-p[1]*i);
		jac[j++]=1.0;
	}
}

void levDemo()
{
	const int n=400, m=3; // 400 measurements, 3 parameters
	double p[m], x[n], opts[LM_OPTS_SZ], info[LM_INFO_SZ];
	int i;
	int ret;

	// generate some measurement using the exponential model with parameters
	// (5.0, 0.1, 1.0), corrupted with zero-mean Gaussian noise of s=0.1
 
	srand(10);
	for(i=0; i<n; ++i)
	{
		x[i]=(5.0*exp(-0.1*i) + 1.0) + gNoise(0.0, 0.0);
	}

	// initial parameters estimate: (1.0, 0.0, 0.0)
	p[0]=1.0; p[1]=0.0; p[2]=0.0;

	// optimization control parameters; passing to levmar NULL instead of opts reverts to defaults
	opts[0]=LM_INIT_MU;
	opts[1]=1E-15;
	opts[2]=1E-15;
	opts[3]=1E-20;
	opts[4]=LM_DIFF_DELTA; // relevant only if the finite difference Jacobian version is used 

	// invoke the optimization function
	ret=dlevmar_der(expfunc, jacexpfunc, p, x, m, n, 1000, opts, info, NULL, NULL, NULL); // with analytic Jacobian
	//ret=dlevmar_dif(expfunc, p, x, m, n, 1000, opts, info, NULL, NULL, NULL); // without Jacobian
	printf("Levenberg-Marquardt returned in %g iter, reason %g, sumsq %g [%g]\n", info[5], info[6], info[1], info[0]);
	printf("Best fit parameters: %.7g %.7g %.7g\n", p[0], p[1], p[2]);
}