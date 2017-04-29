#include <conio.h>
#include <iostream>
#include <iomanip>

#include "TaylorExpansion.h"
#include "mpir.h"
#include "asa241.h"
#include "asa066.h"


using namespace std;


double mysqrt(double d, int n)
{
	double un = d/2.0;

	for (int i=0; i<n; ++i)
		un = 0.5 * (un+d/un);
	return un;
}


void mpfsqrt(mpf_t& res, double _d, int n, int precision)
{
	mpf_t un, aux, d;
	mpf_init2 (un, precision);
	mpf_init2 (aux, precision);
	mpf_init2 (d, precision);

	mpf_init_set_d (un, _d/2.0);
	mpf_set_d (d, _d);

	for (int i=0; i<n; ++i)
	{
		// un = 0.5 * (un+d/un);
		mpf_div (aux, d, un);
		mpf_add (un, un, aux);
		mpf_div_ui (un, un, 2);
	}
	mpf_set(res, un);

	mpf_clears (aux, un, d);
} // void mpfsqrt(mpf_t& res, double d, int n)


void testSqrt()
{
	double x = 3;
	int iterations = 15;
	int precision = 50;

	cout<<"Double arithmetic: " <<setprecision(16)<<mysqrt(x, iterations)<<endl;

	mpf_t res;
	mpf_init2 (res, precision); 

	mpf_t op;
	mpf_init2 (op, precision); 
	mpf_init_set_d (op, x);

	mpf_sqrt (res, op);

	gmp_printf ("Built in MPIR value %.*Ff with %d digits\n", precision, res, precision);

	mpf_clear (op);

	mpfsqrt(res, x, iterations, precision);
	gmp_printf ("Bespoke series expansion value %.*Ff with %d digits\n", precision, *res, precision);
	mpf_clear (res);
} // void testSqrt()


void testTaylor()
{
	int precision = 150; // in bits

	mpf_t res;
	mpf_init2 (res, precision);

	cosine cs;
	cs.initCoeffs(80, precision);
//	double x = 1.5707963267948966192313216916398; //pi/2
	double x = 50; 
	cs.evaluate(res, x); 
	//pi/3 = 1.0471975511966000000

	gmp_printf ("cos(x)= %.*Ff with %d bits\n", precision, *res, precision);
//	printf ("cos(x)= %f\n", cos(1.0471975511966000000));
	cout<<setprecision(16)<<"cos("<<x<<")="<<cos(x)<<endl;


	exponential expo;
	expo.initCoeffs(25, precision);
	x = 1; 
	expo.evaluate(res, x); 
	gmp_printf ("exp(x)= %.*Ff with %d bits\n", precision, *res, precision);
	cout<<setprecision(16)<<"exp("<<x<<")="<<exp(x)<<endl;


	inverf inverf;
	inverf.initCoeffs(25, precision);
	x = 0.5/2*sqrt(3.14159265); 
	inverf.evaluate(res, x); 
	gmp_printf ("inverf(x)= %.*Ff with %d bits\n", precision, *res, precision);

	mpf_clear (res);
//	computeErfCoefficients();
}


void asa_cdf_test()
{

	float f = 0;
	double d = 0;
	

	cout<<alnorm(-10, false)<<endl;

	for (int i=-10; i<10; ++i)
	{
		d = alnorm(i, false);
		cout<<i<< "  "<<d<<"   "<< setprecision(8)<<r4_normal_01_cdf_inverse((float)d)<<"   "<<setprecision(16)<<r8_normal_01_cdf_inverse(d)<<endl;

//		cout<<i<< "  "<<setprecision(8)<<r4_normal_01_cdf_inverse(f)<<"   "<<setprecision(16)<<r8_normal_01_cdf_inverse(d)<<endl;
		f = f+FLT_MIN;
		d = d+FLT_MIN;
	}
}




void main()
{
//	testSqrt();
	testTaylor();

//	asa241Test ( );
//	asa_cdf_test();


	cout<<"Terminated. Press any key to continue."<<endl;
	_getch();
} // void main()