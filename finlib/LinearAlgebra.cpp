#include <gsl/gsl_eigen.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_vector.h>

//#include "Matrix.h"

void eigen(double * dataPtr, int n, gsl_vector *eval, gsl_matrix *evec)
// dataPtr points to n*n matrix, contigues storage
{
	gsl_matrix_view m = gsl_matrix_view_array (dataPtr, n, n);
	gsl_eigen_symmv_workspace * w = gsl_eigen_symmv_alloc (n);
	gsl_eigen_symmv (&m.matrix, eval, evec, w);
	gsl_eigen_symmv_free (w);
	gsl_eigen_symmv_sort (eval, evec, GSL_EIGEN_SORT_ABS_ASC);
} // eigen


