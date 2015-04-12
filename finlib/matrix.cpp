#include "matrix.h"
#include "myError.h"
#include <boost/shared_ptr.hpp>
#include <boost/numeric/ublas/matrix.hpp>

//using namespace boost::numeric;

typedef boost::numeric::ublas::matrix<double> dmat;
typedef boost::numeric::ublas::matrix<double>::size_type dmat_sizetype;


boost::shared_ptr<dmat> mult (const dmat& x, const dmat& y)
{
	dmat_sizetype xr(x.size1()), xc(x.size2());
	dmat_sizetype yr(y.size1()), yc(y.size2());

	if (xc!=yc) throw myError("Incompatible dimensions for matrix product.");

	boost::shared_ptr<dmat> res(new dmat (xr, yc,0));
	
	dmat& p =*res;

	dmat_sizetype i,j,k;

	// rowmajor assumed

	for (i=0; i<xr; ++i)
		for (j=0; j<yc; ++j)
			for (k=0; i<xc; ++k)
				p(i,j)+= x(i,k) * y(k,j);

	return res;
}