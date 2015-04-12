#ifndef _LINEARALGEBRA_H_
#define _LINEARALGEBRA_H_

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include "Matrix.h"


void eigen(double * dataPtr, int n, gsl_vector *eval, gsl_matrix *evec);

/*void matrix_inverse(gsl_matrix * A)
{
	int gsl_linalg_LU_decomp (gsl_matrix * A, gsl_permutation * p, int * signum)
	int gsl_linalg_LU_invert (const gsl_matrix * LU, const gsl_permutation * p, gsl_matrix * inverse)
or alternatively
	gsl_linalg_cholesky_decomp (gsl_matrix * A)
	gsl_linalg_cholesky_invert (gsl_matrix * cholesky)
}*/


template<class T>
HuMatrix<T> matProd(const typename HuMatrix<T>& m1, const typename HuMatrix<T>& m2)
{
	//matrices are m1(n,m) and m2(m,l)
	unsigned int i,j,k, n,m,l;
	n = m1.size1(); // 
	m = m1.size2(); // 
	if (m2.size1() != m)
	{
		throw ("Matrix dimensions do not match");
	}
	l = m2.size2();
	HuMatrix<T> res (n, l);

	for (i = 0; i<n; ++i )
	{
		for (j = 0; j<l; ++j )
		{
			res(i,j) = 0.0;
			for (k = 0; k<m; ++k )
			{
				double a, b;
				a=m1(i,k);
				b=m2(k,j);
				res(i,j) += m1(i,k)*m2(k,j);
			}
		} // for (j = 0; j<m2.size2(); ++j )
	} // for (i = 0; i<m1.size1(); ++i )
	return res;
} // HuMatrix<T> matProd(const T& m1, const T& m2)

#endif