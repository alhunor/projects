// Downloaded from: https://people.sc.fsu.edu/~jburkardt/c_src/asa241/asa241.html

# include <stdlib.h>
# include <stdio.h>
# include <math.h>
# include <time.h>

# include "asa241.h"


void normal_01_cdf_values ( int *n_data, double *x, double *fx )
/* Purpose: returns some values of the Normal 01 CDF.
  Reference: Milton Abramowitz, Irene Stegun, Handbook of Mathematical Functions, National Bureau of Standards, 1964, ISBN: 0-486-61272-4, LC: QA47.A34.
  Stephen Wolfram, The Mathematica Book, Fourth Edition, Cambridge University Press, 1999, ISBN: 0-521-64314-7, LC: QA76.95.W65.

  Parameters:
    Input/output, int *N_DATA.  The user sets N_DATA to 0 before the first call.  On each call, the routine increments N_DATA by 1, and returns the corresponding data; 
	when there is no more data, the output value of N_DATA will be 0 again.

    Output, double *X, the argument of the function.
    Output, double *FX, the value of the function.
*/
{
# define N_MAX 17

  double fx_vec[N_MAX] = { 
     0.5000000000000000E+00,  
     0.5398278372770290E+00,  
     0.5792597094391030E+00,  
     0.6179114221889526E+00,  
     0.6554217416103242E+00,  
     0.6914624612740131E+00,  
     0.7257468822499270E+00,  
     0.7580363477769270E+00,  
     0.7881446014166033E+00,  
     0.8159398746532405E+00,  
     0.8413447460685429E+00,  
     0.9331927987311419E+00,  
     0.9772498680518208E+00,  
     0.9937903346742239E+00,  
     0.9986501019683699E+00,  
     0.9997673709209645E+00,  
     0.9999683287581669E+00 };

  double x_vec[N_MAX] = { 
     0.0000000000000000E+00,    
     0.1000000000000000E+00,  
     0.2000000000000000E+00,  
     0.3000000000000000E+00,  
     0.4000000000000000E+00,  
     0.5000000000000000E+00,  
     0.6000000000000000E+00,  
     0.7000000000000000E+00,  
     0.8000000000000000E+00,  
     0.9000000000000000E+00,  
     0.1000000000000000E+01,  
     0.1500000000000000E+01,  
     0.2000000000000000E+01,  
     0.2500000000000000E+01,  
     0.3000000000000000E+01,  
     0.3500000000000000E+01,  
     0.4000000000000000E+01 };

  if ( *n_data < 0 )
  {
    *n_data = 0;
  }

  *n_data = *n_data + 1;

  if ( N_MAX < *n_data )
  {
    *n_data = 0;
    *x = 0.0;
    *fx = 0.0;
  }
  else
  {
    *x = x_vec[*n_data-1];
    *fx = fx_vec[*n_data-1];
  }

  return;
# undef N_MAX
}


float r4_huge ( )

//  Purpose: returns a "huge" R4.
// The value returned by this function is NOT required to be the maximum representable R4. This value varies from machine to machine, from compiler to compiler,
// and may cause problems when being printed. We simply want a "very large" but non-infinite number.
{
  float value;

  value = 1.0E+30f;

  return value;
}


float r4_normal_01_cdf_inverse ( float p )
// Purpose: inverts the standard normal CDF. The result is accurate to about 1 part in 10**7.
// Author:  Original FORTRAN77 version by Michael Wichura. C version by John Burkardt.

// Reference: Michael Wichura, The Percentage Points of the Normal Distribution, Algorithm AS 241, Applied Statistics, Volume 37, Number 3, pages 477-484, 1988.

//  Parameters: Input, float P, the value of the cumulative probability densitity function. 0 < P < 1.  If P is outside this range, an "infinite" result is returned.
// Outputs the normal deviate value with the property that the probability of a standard normal deviate being less than or equal to this value is P.
{
  static float a[4] = { 3.3871327179f, 50.434271938f, 159.29113202f, 59.109374720f };
  static float b[4] = { 1.0, 17.895169469f, 78.757757664f, 67.187563600f };
  static float c[4] = { 1.4234372777f, 2.7568153900f, 1.3067284816f, 0.17023821103f };
  static float const1 = 0.180625f;
  static float const2 = 1.6f;
  static float d[3] = { 1.0f, 0.73700164250f, 0.12021132975f };
  static float e[4] = { 6.6579051150f, 3.0812263860f, 0.42868294337f, 0.017337203997f };
  static float f[3] = { 1.0f, 0.24197894225f, 0.012258202635f };
  float q;
  float r;
  static float split1 = 0.425f;
  static float split2 = 5.0f;
  float value;

  if ( p <= 0.0 )
  {
    value = - r4_huge ( );
    return value;
  }

  if ( 1.0 <= p )
  {
    value = r4_huge ( );
    return value;
  }

  q = p - 0.5f;

  if ( fabs ( q ) <= split1 )
  {
    r = const1 - q * q;
    value = q * r4poly_value ( 4, a, r ) / r4poly_value ( 4, b, r );
  }
  else
  {
    if ( q < 0.0 )
    {
      r = p;
    }
    else
    {
      r = 1.0f - p;
    }

    if ( r <= 0.0 )
    {
      value = - 1.0;
      exit ( 1 );
    }

    r = sqrt ( -log ( r ) );

    if ( r <= split2 )
    {
      r = r - const2;
      value = r4poly_value ( 4, c, r ) / r4poly_value ( 3, d, r );
    }
    else
    {
      r = r - split2;
      value = r4poly_value ( 4, e, r ) / r4poly_value ( 3, f, r );
    }

    if ( q < 0.0 )
    {
      value = - value;
    }

  }

  return value;
}

float r4poly_value ( int n, float a[], float x )
// Purpose: evaluates a real polynomial st point x. The value of N indicates the NUMBER of coefficients, or more precisely, the ORDER of the polynomial, rather than the DEGREE of the polynomial. 
// The two quantities differ by 1, but cause a great deal of confusion.
// the form of the polynomial is: p(x) = a[0] + a[1] * x + ... + a[n-2] * x^(n-2) + a[n-1] * x^(n-1)
{
  int i;
  float value;

  value = 0.0;

  for ( i = n - 1; 0 <= i; i-- )
  {
    value = value * x + a[i];
  }

  return value;
}


double r8_huge ( )
//  Purpose: returns a "huge" R8. The value returned by this function is NOT required to be the maximum representable R8.  This value varies from machine to machine, from 
// compiler to compiler, and may cause problems when being printed. We simply want a "very large" but non-infinite number.

{
  double value;

  value = 1.0E+30;

  return value;
}

double r8_normal_01_cdf_inverse ( double p )
//  Purpose: inverts the standard normal CDF. The result is accurate to about 1 part in 10^16.
// Author: Original FORTRAN77 version by Michael Wichura. C version by John Burkardt.
{
  static double a[8] = { 
    3.3871328727963666080,     1.3314166789178437745e+2,
    1.9715909503065514427e+3,  1.3731693765509461125e+4,
    4.5921953931549871457e+4,  6.7265770927008700853e+4,
    3.3430575583588128105e+4,  2.5090809287301226727e+3 };
  static double b[8] = {
    1.0,                       4.2313330701600911252e+1,
    6.8718700749205790830e+2,  5.3941960214247511077e+3,
    2.1213794301586595867e+4,  3.9307895800092710610e+4,
    2.8729085735721942674e+4,  5.2264952788528545610e+3 };
  static double c[8] = {
    1.42343711074968357734,     4.63033784615654529590,
    5.76949722146069140550,     3.64784832476320460504,
    1.27045825245236838258,     2.41780725177450611770e-1,
    2.27238449892691845833e-2,  7.74545014278341407640e-4 };
  static double const1 = 0.180625;
  static double const2 = 1.6;
  static double d[8] = {
    1.0,                        2.05319162663775882187,
    1.67638483018380384940,     6.89767334985100004550e-1,
    1.48103976427480074590e-1,  1.51986665636164571966e-2,
    5.47593808499534494600e-4,  1.05075007164441684324e-9 };
  static double e[8] = {
    6.65790464350110377720,     5.46378491116411436990,
    1.78482653991729133580,     2.96560571828504891230e-1,
    2.65321895265761230930e-2,  1.24266094738807843860e-3,
    2.71155556874348757815e-5,  2.01033439929228813265e-7 };
  static double f[8] = {
    1.0,                        5.99832206555887937690e-1,
    1.36929880922735805310e-1,  1.48753612908506148525e-2,
    7.86869131145613259100e-4,  1.84631831751005468180e-5,
    1.42151175831644588870e-7,  2.04426310338993978564e-15 };
  double q;
  double r;
  static double split1 = 0.425;
  static double split2 = 5.0;
  double value;

  if ( p <= 0.0 )
  {
    value = - r8_huge ( );
    return value;
  }

  if ( 1.0 <= p )
  {
    value = r8_huge ( );
    return value;
  }

  q = p - 0.5;

  if ( fabs ( q ) <= split1 )
  {
    r = const1 - q * q;
    value = q * r8poly_value ( 8, a, r ) / r8poly_value ( 8, b, r );
  }
  else
  {
    if ( q < 0.0 )
    {
      r = p;
    }
    else
    {
      r = 1.0 - p;
    }

    if ( r <= 0.0 )
    {
      value = - 1.0;
      exit ( 1 );
    }

    r = sqrt ( -log ( r ) );

    if ( r <= split2 )
    {
      r = r - const2;
      value = r8poly_value ( 8, c, r ) / r8poly_value ( 8, d, r ); 
     }
     else
     {
       r = r - split2;
       value = r8poly_value ( 8, e, r ) / r8poly_value ( 8, f, r );
    }

    if ( q < 0.0 )
    {
      value = - value;
    }

  }

  return value;
}

double r8poly_value ( int n, double a[], double x )
//  Purpose: evaluates a double precision polynomial at point x. The value of N indicates the NUMBER of coefficients, or more precisely, the ORDER of the polynomial, rather than the DEGREE of the polynomial. 
// The two quantities differ by 1, but cause a great deal of confusion.
// the form of the polynomial is: p(x) = a[0] + a[1] * x + ... + a[n-2] * x^(n-2) + a[n-1] * x^(n-1)
{
  int i;
  double value;

  value = 0.0;

  for ( i = n - 1; 0 <= i; i-- )
  {
    value = value * x + a[i];
  }

  return value;
}


void timestamp ( void )
// TIMESTAMP prints the current YMDHMS date as a time stamp.
{
# define TIME_SIZE 40

  static char time_buffer[TIME_SIZE];
  const struct tm *tm;
  size_t len;
  time_t now;

  now = time ( NULL );
  tm = localtime ( &now );

  len = strftime ( time_buffer, TIME_SIZE, "%d %B %Y %I:%M:%S %p", tm );

  fprintf ( stdout, "%s\n", time_buffer );

  return;
# undef TIME_SIZE
}


void test01 ( )
// Purpose: tests R4_NORMAL_01_CDF_INVERSE, NORMAL_01_CDF_VALUES.
{
  double fx;
  float fx2;
  int n_data;
  double x;
  float x2;

  printf ( "\n" );
  printf ( "TEST01:\n" );
  printf ( "  Let FX = NormalCDF ( X ).\n" );
  printf ( "\n" );
  printf ( "  NORMAL_01_CDF_VALUES returns some values of ( X, FX ).\n" );
  printf ( "\n" );
  printf ( "  R4_NORMAL_01_CDF_INVERSE takes the value of FX, and computes an\n" );
  printf ( "    estimate X2, of the corresponding input argument,\n" );
  printf ( "    accurate to about 7 decimal places.\n" );
  printf ( "\n" );
  printf ( "          FX                        X                        X2          DIFF\n" );
  printf ( "\n" );

  n_data = 0;

  for ( ; ; )
  {
    normal_01_cdf_values ( &n_data, &x, &fx );

    if ( n_data == 0 )
    {
      break;
    }

    fx2 = ( float ) ( fx );
    x2 = r4_normal_01_cdf_inverse ( fx2 );

    printf ( "  %24.16f  %24.16f  %24.16f  %10.6g\n", fx, x, x2, fabs ( x - x2 ) );
  }
}

void test02 ( )
//  Purpose: R8_NORMAL_01_CDF_INVERSE, NORMAL_01_CDF_VALUES.
{
  double fx;
  int n_data;
  double x;
  double x2;

  printf ( "\n" );
  printf ( "TEST02:\n" );
  printf ( "  Let FX = NormalCDF ( X ).\n" );
  printf ( "\n" );
  printf ( "  NORMAL_01_CDF_VALUES returns some values of ( X, FX ).\n" );
  printf ( "\n" );
  printf ( "  R8_NORMAL_01_CDF_INVERSE takes the value of FX, and computes an\n" );
  printf ( "    estimate X2, of the corresponding input argument,\n" );
  printf ( "    accurate to about 16 decimal places.\n" );
  printf ( "\n" );
  printf ( "          FX                        X                        X2          DIFF\n" );
  printf ( "\n" );

  n_data = 0;

  for ( ; ; )
  {
    normal_01_cdf_values ( &n_data, &x, &fx );

    if ( n_data == 0 )
    {
      break;
    }

    x2 = r8_normal_01_cdf_inverse ( fx );

    printf ( "  %24.16f  %24.16f  %24.16f  %10.6g\n", fx, x, x2, fabs ( x - x2 ) );
  }
}



//Purpose: tests the ASA241 library.
void asa241Test ( )
{
  timestamp ( );
  printf ( "\n" );
  printf ( "  Test the ASA241 library.\n" );

  test01 ( );
  test02 ( );

  printf ( "  Normal end of execution.\n" );
  printf ( "\n" );
  timestamp ( );
}
