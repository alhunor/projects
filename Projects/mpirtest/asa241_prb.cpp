// Downloaded from: https://people.sc.fsu.edu/~jburkardt/c_src/asa241/asa241.html

# include <stdlib.h>
# include <stdio.h>
# include <math.h>
# include <time.h>

# include "asa241.h"


int asa241Test ( )

//Purpose: tests the ASA241 library.
{
  timestamp ( );
  printf ( "\n" );
  printf ( "ASA241_PRB:\n" );
  printf ( "  C version\n" );
  printf ( "  Test the ASA241 library.\n" );

  test01 ( );
  test02 ( );

  printf ( "\n" );
  printf ( "ASA241_PRB:\n" );
  printf ( "  Normal end of execution.\n" );
  printf ( "\n" );
  timestamp ( );

  return 0;
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
  double fx2;
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
