#include "mpir.h"
#include "TaylorExpansion.h"
#include <iostream>
//#include <math.h>

using namespace std;

mpf_t TaylorExpansion::flt_min;
mpf_t TaylorExpansion::dbl_min;
bool TaylorExpansion::initialised;

static bool debug = false;

TaylorExpansion::TaylorExpansion() : coeffs(NULL)
{
	if (initialised) return;
	mpf_init2 (flt_min, 23+1);
	mpf_set_d(flt_min, FLT_MIN);
	mpf_init2 (dbl_min, 52+1);
	mpf_set_d(dbl_min, DBL_MIN);
	initialised = true;
}

TaylorExpansion::~TaylorExpansion()
{
	if (! coeffs) return;
	for (unsigned int k=0; k<=nb; ++k)
	{
		mpf_clear(coeffs[k]);
	}
	delete[] coeffs;
	coeffs = NULL;

	mpf_clear(x2);
	mpf_clear(xk);
	mpf_clear(term);
} // TaylorExpansion::~TaylorExpansion()

bool TaylorExpansion::initCoeffs(unsigned int order, unsigned int _precision)
{
	nb = order;
	precision =  _precision;
	//mpf_set_default_prec(precision);
	coeffs = new mpf_t[nb+1];
	for (unsigned int k=0; k<=nb; ++k)
	{
		mpf_init2(coeffs[k], precision);
	}

	mpf_init2 (x2, precision);
	mpf_init2 (xk, precision);
	mpf_init2 (term, precision);

	return true;
} // bool TaylorExpansion::initCoeffs(int order, int _precision)




bool exponential::initCoeffs(unsigned int order, unsigned int precision)
{
	if ( ! TaylorExpansion::initCoeffs(order, precision)) return false;

	// calculate coefficients for cosine. coeff[n] = 1 / n!
	// exp(x) = coeff[0] + coeff[1]*x +..+ coeff[n]*x^n
	unsigned int k;
	mpz_t denominator, z;
	mpz_inits(denominator, z);
	mpz_set_d(denominator,1);

	mpq_t coeff_k;
	mpq_init(coeff_k);

	mpz_init(z);

	mpf_set_d (coeffs[0], 1);
	for (k=1; k<=nb; ++k)
	{
		mpz_set_ui(z, k);
		mpz_mul(denominator, denominator, z);

		mpq_set_z(coeff_k, denominator);
		mpq_inv(coeff_k, coeff_k);

		if (debug) gmp_printf ("coeffs[%d] = %Qd\n", k, coeff_k);
		mpf_set_q(coeffs[k], coeff_k);
	}

	mpz_clear(denominator);
	mpz_clear(z);
	mpq_clear(coeff_k);
	return true;
} // bool exponential::initCoeffs(int order)




bool cosine::initCoeffs(unsigned int order, unsigned int precision)
{
	if ( ! TaylorExpansion::initCoeffs(order, precision)) return false;

	// calculate coefficients for cosine. coeff[n] = (-1)^n / (2n)!
	// cos(x) = coeff[0] + coeff[1]*x^2 + coeff[n]*x^(2n)
	unsigned int k;
	mpz_t denominator, z;
	mpz_inits(denominator, z);
	mpz_set_d(denominator,1);

	mpq_t coeff_k;
	mpq_init(coeff_k);

	mpz_init(z);

	mpf_set_d (coeffs[0], 1);
	for (k=1; k<=nb; ++k)
	{
		mpz_set_ui(z, 2*k-1);
		mpz_mul(denominator, denominator, z);
		mpz_set_ui(z, 2*k);
		mpz_mul(denominator, denominator, z);

		mpq_set_z(coeff_k, denominator);
		mpq_inv(coeff_k, coeff_k);

		if ((k % 2)) mpq_neg (coeff_k, coeff_k); // negate odd coeffs
		if (debug) gmp_printf ("coeffs[%d] = %Qd\n", k, coeff_k);
		mpf_set_q(coeffs[k], coeff_k);
	}

	mpz_clear(denominator);
	mpz_clear(z);
	mpq_clear(coeff_k);
	return true;
} // bool cosine::initCoeffs(int order)



bool sine::initCoeffs(unsigned int order, unsigned int precision)
{
	if ( ! TaylorExpansion::initCoeffs(order, precision)) return false;

	// calculate coefficients for sine. coeff[n] = (-1)^(n) / (2n+1)!
	// all positions are shifted to the left by 1 index, e.g sin(x) = coeff[0]*x + coeff[1]*x^3 + coeff[n]*x^(2n+1)
	unsigned int k;
	mpq_t coeff_k;

	mpz_t denominator, z;
	mpz_inits(denominator, z);
	mpz_set_d(denominator,1);
	
	mpq_init(coeff_k);

	mpf_set_d (coeffs[0], 1);

	for (k=1; k<=nb; ++k)
	{
		mpz_set_ui(z, 2*k);
		mpz_mul(denominator, denominator, z);
		mpz_set_ui(z, 2*k+1);
		mpz_mul(denominator, denominator, z);

		mpq_set_z(coeff_k, denominator);
		mpq_inv(coeff_k, coeff_k);

		if ((k % 2)) mpq_neg (coeff_k, coeff_k); // negate every second term
		if (debug) gmp_printf ("coeffs[%d] = %Qd\n", k, coeffs[k]);
		mpf_set_q(coeffs[k], coeff_k);
	}
	mpz_clear(denominator);
	mpz_clear(z);
	mpq_clear(coeff_k);
	return true;
} // bool sine::initCoeffs(int order)



bool TaylorExpansion::evaluate(mpf_t& res, mpf_t x)
{
	if (! coeffs) return false;
	
	unsigned int k;
	mpf_set(x2, x);
	bool floatPrecisionReached = false;
	bool doublePrecisionReached = false;
	double d;

	if (summationType == Even) // // res = coeff[0] + x^2*coeff[1] + .. + x^(2k)*coeff[k]
	{
		mpf_mul(x2, x2, x2); // // x2 == x^2 at this stage
		mpf_set_ui (xk, 1);
		mpf_set(res, coeffs[0]);
		d = mpf_get_d(res);
		for (k=1; k<=nb; ++k)
		{
			mpf_mul(xk, xk, x2);
			d = mpf_get_d(xk);
				mpf_mul(term, coeffs[k], xk);
			d = mpf_get_d(term);
			cout<<k<<" --->" <<d<<endl;
			//gmp_printf ("%.*Ff\n", precision, res);

			mpf_add(res, res, term);
			d = mpf_get_d(res);

			mpf_abs(term, term);
			if (mpf_cmp(term, flt_min)<0 && !floatPrecisionReached )
			{
				gmp_printf ("Reached float precision after %d terms.\n", k);
				floatPrecisionReached  = true;
			}
			if (mpf_cmp(term, dbl_min)<0 && !doublePrecisionReached )
			{
				gmp_printf ("Reached double precision after %d terms.\n", k);
				doublePrecisionReached  = true;
			}
		} // for (int k=1; k<=n; ++k)
	} else if (summationType == Odd) // res = x* coeff[0] + x^3*coeff[1] + .. + x^(2k+1)*coeff[k]
	{
		mpf_set (xk, x); // xk == x at this stage
		mpf_mul(x2, x2, x2); // x2 == x^2 at this stage
		mpf_mul(res, coeffs[0], xk);
		for (k=1; k<=nb; ++k)
		{
			mpf_mul(xk, xk, x2);
			mpf_mul(term, coeffs[k], xk);
			mpf_add(res, res, term);
		} // for (int k=1; k<=n; ++k)	
	} else  if (summationType == All) // res = coeff[0] + x* coeff[1] + x^2*coeff[2] + .. + x^k*coeff[k]
	{
		mpf_set(res, coeffs[0]);
		mpf_set (xk, x); // x2 == x at this stage
		mpf_set_ui (xk, 1);
		for (k=1; k<=nb; ++k)
		{
			mpf_mul(xk, xk, x2);
			mpf_mul(term, coeffs[k], xk);
			mpf_add(res, res, term);
			// step to next term
		} // for (int k=1; k<=n; ++k)	
	}
	return true;
} // bool TaylorExpansion::evaluate(mpf_t& res, double x)



bool TaylorExpansion::evaluate(mpf_t& res, double x)
{
	if (! coeffs) return false;
	mpf_t temp;
	mpf_init2(temp, precision);
	mpf_set_d(temp, x);
	bool b = evaluate(res, temp);
	mpf_clear(temp);
	return b;
} // bool TaylorExpansion::evaluate(mpf_t& res, double x)





bool inverf::initCoeffs(unsigned int order, unsigned int precision)
{
	if ( ! TaylorExpansion::initCoeffs(order, precision)) return false;

	mpq_t an, *c, aux, term;
	unsigned int n, k;

	mpq_init(aux);
	mpq_init(term);

	c = new mpq_t[nb+1];

	for (n = 0; n<=nb; ++n) mpq_init(c[n]);
	mpq_init(an);

	mpq_set_ui(c[0], 1,1);
	mpq_set_ui(c[1], 1,1);

	mpq_set_ui(an, 1,1);
	gmp_printf ("a[0]= %Qd\n", an);
	mpf_set_q(coeffs[0], an);
	mpq_set_ui(an, 1,3);
	gmp_printf ("a[1]= %Qd\n", an);
	mpf_set_q(coeffs[1], an);

	for (n = 2; n<=nb; ++n)
	{
		for (k = 0; k<=n-1; ++k)
		{
			mpq_mul(term, c[k], c[n-k-1]);
			mpq_set_ui(aux, (k+1)*(2*k+1), 1);
			mpq_div(term, term, aux);
			mpq_add(c[n], c[n], term);
		}
		mpq_set_ui(aux, 2*n+1, 1);
		mpq_div(an, c[n], aux);
	//	gmp_printf ("%dth position : a[n]= %Qd c[n] = %Qd\n", n, a[n], c[n]);
		gmp_printf ("a[%d]= %Qd\n", n, an);
		mpf_set_q(coeffs[n],an);
	}

	for (n = 0; n<=nb; ++n) mpq_clear(c[n]);
	mpq_clear(an);
	delete[] c;
	mpq_clear(aux);
	mpq_clear(term);
	return true;
} // bool inverf::initCoeffs(unsigned int order, unsigned int precision)


void computeErfCoefficients()
{
// Numerator from http://oeis.org/A092676 , numeraires http://oeis.org/A092677
// recursive formula http://mathworld.wolfram.com/InverseErf.html

	// computes first N coefficients
	const int N = 15;
	const int refN = 24;
	int precion = 2;
	mpq_t a[N+1], c[N+1], ref[refN+1], aux, term;
	int n, k;

	mpq_init(aux);
	mpq_init(term);

	for (n = 0; n<=N; ++n)
	{
		mpq_init(a[n]);
		mpq_init(c[n]);
	}

	mpq_set_ui(c[0], 1,1);
	mpq_set_ui(c[1], 1,1);
	mpq_set_ui(a[0], 1,1);
	mpq_set_ui(a[1], 1,3);

	//gmp_printf ("Ratio is %Qd\n", a[1]);
	gmp_printf ("a[0]= %Qd\n", a[0]);
	gmp_printf ("a[1]= %Qd\n", a[1]);

	for (n = 2; n<=N; ++n)
	{
		//c[n] is zero, after init
		for (k = 0; k<=n-1; ++k)
		{
			mpq_mul(term, c[k], c[n-k-1]);
		//	gmp_printf ("term = %Qd\n", term);
			mpq_set_ui(aux, (k+1)*(2*k+1), 1);
		//	gmp_printf ("aux = %Qd\n", aux);
			mpq_div(term, term, aux);
		//	gmp_printf ("term = %Qd\n", term);
			mpq_add(c[n], c[n], term);
		//	gmp_printf ("c[n] = %Qd\n", c[n]);
		}
		mpq_set_ui(aux, 2*n+1, 1);
		mpq_div(a[n], c[n], aux);
	//	gmp_printf ("%dth position : a[n]= %Qd c[n] = %Qd\n", n, a[n], c[n]);
		gmp_printf ("a[%d]= %Qd\n", n, a[n]);
	}

	// These coeffs are form http://oeis.org/A092676/a092676.txt
	for (n = 0; n<=refN; ++n) mpq_init(ref[n]);
	mpq_set_ui(ref[ 0], 1,1);
	mpq_set_ui(ref[ 1], 1,3);
	mpq_set_ui(ref[ 2], 7,30);
	mpq_set_ui(ref[ 3], 127,630);
	mpq_set_ui(ref[ 4], 4369,22680);
	mpq_set_ui(ref[ 5], 34807,178200);
	mpq_set_ui(ref[ 6], 20036983,97297200);
	mpq_set_str(ref[ 7], "2280356863/10216206000",10);
	mpq_set_str(ref[ 8], "49020204823/198486288000",10);
	mpq_set_str(ref[ 9], "65967241200001/237588086736000",10);
	mpq_set_str(ref[10], "15773461423793767/49893498214560000",10);
	mpq_set_str(ref[11], "655889589032992201/1803293578326240000",10);
	mpq_set_str(ref[12], "94020690191035873697/222759794969712000000",10);
	mpq_set_str(ref[13], "655782249799531714375489/1329207696584271504000000",10);
	mpq_set_str(ref[14], "44737200694996264619809969/77094046401887747232000000",10);
	mpq_set_str(ref[15], "10129509912509255673830968079/14761242414008506896480000000",10);
	mpq_set_str(ref[16], "108026349476762041127839800617281/132496911908140357902804480000000",10);
	mpq_set_str(ref[17], "10954814567103825758202995557819063/11262237512191930421738380800000000",10);
	mpq_set_str(ref[18], "61154674195324330125295778531172438727/52504551281838779626144331289600000000",10);
	mpq_set_str(ref[19], "54441029530574028687402753586278549396607/38905872499842535702972949485593600000000",10);
	mpq_set_str(ref[20], "452015832786609665624579410056180824562551/268090886133368733415443853598208000000000",10);
	mpq_set_str(ref[21], "2551405765475004343830620568825540664310892263/1252532276140582782027102181569679872000000000",10);
	mpq_set_str(ref[22], "70358041406630998834159902148730577164631303295543/28520159927721069946757116674341610685440000000000",10);
	mpq_set_str(ref[23], "775752883029173334450858052496704319194646607263417/259078091444256105986928093487086396226560000000000",10);
	mpq_set_str(ref[24], "132034545522738294934559794712527229683368402215775110881/36256424429074976496234665114956818633529712640000000000",10);
	for (n = 0; n<=std::min(N, refN); ++n)
	{
		if ( mpq_equal(ref[n], a[n]) )
		{
			gmp_printf ("Position [%d] matched.\n", n);
		} else
		{
			gmp_printf ("Position [%d] not matched.\n", n);
			gmp_printf ("a[%d]= %Qd\n", n, a[n]);
			gmp_printf ("ref[%d]= %Qd\n", n, ref[n]);
		}
	}
	for (n = 0; n<=refN; ++n) mpq_clear(ref[n]);

	for (n = 0; n<=N; ++n)
	{
		mpq_clear(a[n]);
		mpq_clear(c[n]);
	}

	mpq_clear(aux);
	mpq_clear(term);
} // void computeErfCoefficients()